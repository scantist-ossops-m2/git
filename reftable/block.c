/*
Copyright 2020 Google LLC

Use of this source code is governed by a BSD-style
license that can be found in the LICENSE file or at
https://developers.google.com/open-source/licenses/bsd
*/

#include "block.h"

#include "blocksource.h"
#include "constants.h"
#include "record.h"
#include "reftable-error.h"
#include "system.h"
#include <zlib.h>

int header_size(int version)
{
	switch (version) {
	case 1:
		return 24;
	case 2:
		return 28;
	}
	abort();
}

int footer_size(int version)
{
	switch (version) {
	case 1:
		return 68;
	case 2:
		return 72;
	}
	abort();
}

static int block_writer_register_restart(struct block_writer *w, int n,
					 int is_restart, struct strbuf *key)
{
	int rlen = w->restart_len;
	if (rlen >= MAX_RESTARTS) {
		is_restart = 0;
	}

	if (is_restart) {
		rlen++;
	}
	if (2 + 3 * rlen + n > w->block_size - w->next)
		return -1;
	if (is_restart) {
		REFTABLE_ALLOC_GROW(w->restarts, w->restart_len + 1, w->restart_cap);
		w->restarts[w->restart_len++] = w->next;
	}

	w->next += n;

	strbuf_reset(&w->last_key);
	strbuf_addbuf(&w->last_key, key);
	w->entries++;
	return 0;
}

void block_writer_init(struct block_writer *bw, uint8_t typ, uint8_t *buf,
		       uint32_t block_size, uint32_t header_off, int hash_size)
{
	bw->buf = buf;
	bw->hash_size = hash_size;
	bw->block_size = block_size;
	bw->header_off = header_off;
	bw->buf[header_off] = typ;
	bw->next = header_off + 4;
	bw->restart_interval = 16;
	bw->entries = 0;
	bw->restart_len = 0;
	bw->last_key.len = 0;
}

uint8_t block_writer_type(struct block_writer *bw)
{
	return bw->buf[bw->header_off];
}

/* Adds the reftable_record to the block. Returns -1 if it does not fit, 0 on
   success. Returns REFTABLE_API_ERROR if attempting to write a record with
   empty key. */
int block_writer_add(struct block_writer *w, struct reftable_record *rec)
{
	struct strbuf empty = STRBUF_INIT;
	struct strbuf last =
		w->entries % w->restart_interval == 0 ? empty : w->last_key;
	struct string_view out = {
		.buf = w->buf + w->next,
		.len = w->block_size - w->next,
	};

	struct string_view start = out;

	int is_restart = 0;
	struct strbuf key = STRBUF_INIT;
	int n = 0;
	int err = -1;

	reftable_record_key(rec, &key);
	if (!key.len) {
		err = REFTABLE_API_ERROR;
		goto done;
	}

	n = reftable_encode_key(&is_restart, out, last, key,
				reftable_record_val_type(rec));
	if (n < 0)
		goto done;
	string_view_consume(&out, n);

	n = reftable_record_encode(rec, out, w->hash_size);
	if (n < 0)
		goto done;
	string_view_consume(&out, n);

	err = block_writer_register_restart(w, start.len - out.len, is_restart,
					    &key);
done:
	strbuf_release(&key);
	return err;
}

int block_writer_finish(struct block_writer *w)
{
	int i;
	for (i = 0; i < w->restart_len; i++) {
		put_be24(w->buf + w->next, w->restarts[i]);
		w->next += 3;
	}

	put_be16(w->buf + w->next, w->restart_len);
	w->next += 2;
	put_be24(w->buf + 1 + w->header_off, w->next);

	if (block_writer_type(w) == BLOCK_TYPE_LOG) {
		int block_header_skip = 4 + w->header_off;
		uLongf src_len = w->next - block_header_skip;
		uLongf dest_cap = src_len * 1.001 + 12;
		uint8_t *compressed;

		REFTABLE_ALLOC_ARRAY(compressed, dest_cap);

		while (1) {
			uLongf out_dest_len = dest_cap;
			int zresult = compress2(compressed, &out_dest_len,
						w->buf + block_header_skip,
						src_len, 9);
			if (zresult == Z_BUF_ERROR && dest_cap < LONG_MAX) {
				dest_cap *= 2;
				compressed =
					reftable_realloc(compressed, dest_cap);
				if (compressed)
					continue;
			}

			if (Z_OK != zresult) {
				reftable_free(compressed);
				return REFTABLE_ZLIB_ERROR;
			}

			memcpy(w->buf + block_header_skip, compressed,
			       out_dest_len);
			w->next = out_dest_len + block_header_skip;
			reftable_free(compressed);
			break;
		}
	}
	return w->next;
}

int block_reader_init(struct block_reader *br, struct reftable_block *block,
		      uint32_t header_off, uint32_t table_block_size,
		      int hash_size)
{
	uint32_t full_block_size = table_block_size;
	uint8_t typ = block->data[header_off];
	uint32_t sz = get_be24(block->data + header_off + 1);
	int err = 0;
	uint16_t restart_count = 0;
	uint32_t restart_start = 0;
	uint8_t *restart_bytes = NULL;

	reftable_block_done(&br->block);

	if (!reftable_is_block_type(typ)) {
		err =  REFTABLE_FORMAT_ERROR;
		goto done;
	}

	if (typ == BLOCK_TYPE_LOG) {
		uint32_t block_header_skip = 4 + header_off;
		uLong dst_len = sz - block_header_skip;
		uLong src_len = block->len - block_header_skip;

		/* Log blocks specify the *uncompressed* size in their header. */
		REFTABLE_ALLOC_GROW(br->uncompressed_data, sz,
				    br->uncompressed_cap);

		/* Copy over the block header verbatim. It's not compressed. */
		memcpy(br->uncompressed_data, block->data, block_header_skip);

		if (!br->zstream) {
			REFTABLE_CALLOC_ARRAY(br->zstream, 1);
			err = inflateInit(br->zstream);
		} else {
			err = inflateReset(br->zstream);
		}
		if (err != Z_OK) {
			err = REFTABLE_ZLIB_ERROR;
			goto done;
		}

		br->zstream->next_in = block->data + block_header_skip;
		br->zstream->avail_in = src_len;
		br->zstream->next_out = br->uncompressed_data + block_header_skip;
		br->zstream->avail_out = dst_len;

		/*
		 * We know both input as well as output size, and we know that
		 * the sizes should never be bigger than `uInt_MAX` because
		 * blocks can at most be 16MB large. We can thus use `Z_FINISH`
		 * here to instruct zlib to inflate the data in one go, which
		 * is more efficient than using `Z_NO_FLUSH`.
		 */
		err = inflate(br->zstream, Z_FINISH);
		if (err != Z_STREAM_END) {
			err = REFTABLE_ZLIB_ERROR;
			goto done;
		}
		err = 0;

		if (br->zstream->total_out + block_header_skip != sz) {
			err = REFTABLE_FORMAT_ERROR;
			goto done;
		}

		/* We're done with the input data. */
		reftable_block_done(block);
		block->data = br->uncompressed_data;
		block->len = sz;
		full_block_size = src_len + block_header_skip - br->zstream->avail_in;
	} else if (full_block_size == 0) {
		full_block_size = sz;
	} else if (sz < full_block_size && sz < block->len &&
		   block->data[sz] != 0) {
		/* If the block is smaller than the full block size, it is
		   padded (data followed by '\0') or the next block is
		   unaligned. */
		full_block_size = sz;
	}

	restart_count = get_be16(block->data + sz - 2);
	restart_start = sz - 2 - 3 * restart_count;
	restart_bytes = block->data + restart_start;

	/* transfer ownership. */
	br->block = *block;
	block->data = NULL;
	block->len = 0;

	br->hash_size = hash_size;
	br->block_len = restart_start;
	br->full_block_size = full_block_size;
	br->header_off = header_off;
	br->restart_count = restart_count;
	br->restart_bytes = restart_bytes;

done:
	return err;
}

void block_reader_release(struct block_reader *br)
{
	inflateEnd(br->zstream);
	reftable_free(br->zstream);
	reftable_free(br->uncompressed_data);
	reftable_block_done(&br->block);
}

uint8_t block_reader_type(const struct block_reader *r)
{
	return r->block.data[r->header_off];
}

int block_reader_first_key(const struct block_reader *br, struct strbuf *key)
{
	int off = br->header_off + 4, n;
	struct string_view in = {
		.buf = br->block.data + off,
		.len = br->block_len - off,
	};
	uint8_t extra = 0;

	strbuf_reset(key);

	n = reftable_decode_key(key, &extra, in);
	if (n < 0)
		return n;
	if (!key->len)
		return REFTABLE_FORMAT_ERROR;

	return 0;
}

static uint32_t block_reader_restart_offset(const struct block_reader *br, int i)
{
	return get_be24(br->restart_bytes + 3 * i);
}

void block_iter_seek_start(struct block_iter *it, const struct block_reader *br)
{
	it->block = br->block.data;
	it->block_len = br->block_len;
	it->hash_size = br->hash_size;
	strbuf_reset(&it->last_key);
	it->next_off = br->header_off + 4;
}

struct restart_needle_less_args {
	int error;
	struct strbuf needle;
	const struct block_reader *reader;
};

static int restart_needle_less(size_t idx, void *_args)
{
	struct restart_needle_less_args *args = _args;
	uint32_t off = block_reader_restart_offset(args->reader, idx);
	struct string_view in = {
		.buf = args->reader->block.data + off,
		.len = args->reader->block_len - off,
	};
	uint64_t prefix_len, suffix_len;
	uint8_t extra;
	int n;

	/*
	 * Records at restart points are stored without prefix compression, so
	 * there is no need to fully decode the record key here. This removes
	 * the need for allocating memory.
	 */
	n = reftable_decode_keylen(in, &prefix_len, &suffix_len, &extra);
	if (n < 0 || prefix_len) {
		args->error = 1;
		return -1;
	}

	string_view_consume(&in, n);
	if (suffix_len > in.len) {
		args->error = 1;
		return -1;
	}

	n = memcmp(args->needle.buf, in.buf,
		   args->needle.len < suffix_len ? args->needle.len : suffix_len);
	if (n)
		return n < 0;
	return args->needle.len < suffix_len;
}

void block_iter_copy_from(struct block_iter *dest, const struct block_iter *src)
{
	dest->block = src->block;
	dest->block_len = src->block_len;
	dest->hash_size = src->hash_size;
	dest->next_off = src->next_off;
	strbuf_reset(&dest->last_key);
	strbuf_addbuf(&dest->last_key, &src->last_key);
}

int block_iter_next(struct block_iter *it, struct reftable_record *rec)
{
	struct string_view in = {
		.buf = (unsigned char *) it->block + it->next_off,
		.len = it->block_len - it->next_off,
	};
	struct string_view start = in;
	uint8_t extra = 0;
	int n = 0;

	if (it->next_off >= it->block_len)
		return 1;

	n = reftable_decode_key(&it->last_key, &extra, in);
	if (n < 0)
		return -1;
	if (!it->last_key.len)
		return REFTABLE_FORMAT_ERROR;

	string_view_consume(&in, n);
	n = reftable_record_decode(rec, it->last_key, extra, in, it->hash_size,
				   &it->scratch);
	if (n < 0)
		return -1;
	string_view_consume(&in, n);

	it->next_off += start.len - in.len;
	return 0;
}

void block_iter_reset(struct block_iter *it)
{
	strbuf_reset(&it->last_key);
	it->next_off = 0;
	it->block = NULL;
	it->block_len = 0;
	it->hash_size = 0;
}

void block_iter_close(struct block_iter *it)
{
	strbuf_release(&it->last_key);
	strbuf_release(&it->scratch);
}

int block_iter_seek_key(struct block_iter *it, const struct block_reader *br,
			struct strbuf *want)
{
	struct restart_needle_less_args args = {
		.needle = *want,
		.reader = br,
	};
	struct block_iter next = BLOCK_ITER_INIT;
	struct reftable_record rec;
	int err = 0;
	size_t i;

	/*
	 * Perform a binary search over the block's restart points, which
	 * avoids doing a linear scan over the whole block. Like this, we
	 * identify the section of the block that should contain our key.
	 *
	 * Note that we explicitly search for the first restart point _greater_
	 * than the sought-after record, not _greater or equal_ to it. In case
	 * the sought-after record is located directly at the restart point we
	 * would otherwise start doing the linear search at the preceding
	 * restart point. While that works alright, we would end up scanning
	 * too many record.
	 */
	i = binsearch(br->restart_count, &restart_needle_less, &args);
	if (args.error) {
		err = REFTABLE_FORMAT_ERROR;
		goto done;
	}

	/*
	 * Now there are multiple cases:
	 *
	 *   - `i == 0`: The wanted record must be contained before the first
	 *     restart point. We will thus start searching for the record in
	 *     that section after accounting for the header offset.
	 *
	 *   - `i == restart_count`: The wanted record was not found at any of
	 *     the restart points. As there is no restart point at the end of
	 *     the section the record may thus be contained in the last block.
	 *
	 *   - `i > 0`: The wanted record must be contained in the section
	 *     before the found restart point. We thus do a linear search
	 *     starting from the preceding restart point.
	 */
	if (i > 0)
		it->next_off = block_reader_restart_offset(br, i - 1);
	else
		it->next_off = br->header_off + 4;
	it->block = br->block.data;
	it->block_len = br->block_len;
	it->hash_size = br->hash_size;

	reftable_record_init(&rec, block_reader_type(br));

	/*
	 * We're looking for the last entry less than the wanted key so that
	 * the next call to `block_reader_next()` would yield the wanted
	 * record. We thus don't want to position our reader at the sought
	 * after record, but one before. To do so, we have to go one entry too
	 * far and then back up.
	 */
	while (1) {
		block_iter_copy_from(&next, it);
		err = block_iter_next(&next, &rec);
		if (err < 0)
			goto done;
		if (err > 0) {
			err = 0;
			goto done;
		}

		/*
		 * Check whether the current key is greater or equal to the
		 * sought-after key. In case it is greater we know that the
		 * record does not exist in the block and can thus abort early.
		 * In case it is equal to the sought-after key we have found
		 * the desired record.
		 */
		reftable_record_key(&rec, &it->last_key);
		if (strbuf_cmp(&it->last_key, want) >= 0)
			goto done;

		block_iter_copy_from(it, &next);
	}

done:
	block_iter_close(&next);
	reftable_record_release(&rec);

	return err;
}

void block_writer_release(struct block_writer *bw)
{
	FREE_AND_NULL(bw->restarts);
	strbuf_release(&bw->last_key);
	/* the block is not owned. */
}

void reftable_block_done(struct reftable_block *blockp)
{
	struct reftable_block_source source = blockp->source;
	if (blockp && source.ops)
		source.ops->return_block(source.arg, blockp);
	blockp->data = NULL;
	blockp->len = 0;
	blockp->source.ops = NULL;
	blockp->source.arg = NULL;
}
