			DIFF_OPT_CLR(options, DIRSTAT_BY_LINE);
			DIFF_OPT_CLR(options, DIRSTAT_BY_FILE);
			DIFF_OPT_SET(options, DIRSTAT_BY_LINE);
			DIFF_OPT_CLR(options, DIRSTAT_BY_FILE);
			DIFF_OPT_CLR(options, DIRSTAT_BY_LINE);
			DIFF_OPT_SET(options, DIRSTAT_BY_FILE);
			DIFF_OPT_CLR(options, DIRSTAT_CUMULATIVE);
			DIFF_OPT_SET(options, DIRSTAT_CUMULATIVE);
	diff_detect_rename_default = 1;
	if (starts_with(var, "submodule."))
		return parse_submodule_config_option(var, value);

	struct tempfile tempfile;
typedef unsigned long (*sane_truncate_fn)(char *line, unsigned long len);

	sane_truncate_fn truncate;
static int new_blank_line_at_eof(struct emit_callback *ecbdata, const char *line, int len)
	if (!((ecbdata->ws_rule & WS_BLANK_AT_EOF) &&
	      ecbdata->blank_at_eof_in_preimage &&
	      ecbdata->blank_at_eof_in_postimage &&
	      ecbdata->blank_at_eof_in_preimage <= ecbdata->lno_in_preimage &&
	      ecbdata->blank_at_eof_in_postimage <= ecbdata->lno_in_postimage))
		return 0;
	return ws_blank_line(line, len, ecbdata->ws_rule);
static void emit_line_checked(const char *reset,
			      struct emit_callback *ecbdata,
			      const char *line, int len,
			      enum color_diff color,
			      unsigned ws_error_highlight,
			      char sign)
	const char *set = diff_get_color(ecbdata->color_diff, color);
	if (ecbdata->opt->ws_error_highlight & ws_error_highlight) {
		ws = diff_get_color(ecbdata->color_diff, DIFF_WHITESPACE);
		emit_line_0(ecbdata->opt, set, reset, sign, line, len);
	else if (sign == '+' && new_blank_line_at_eof(ecbdata, line, len))
		emit_line_0(ecbdata->opt, ws, reset, sign, line, len);
		emit_line_0(ecbdata->opt, set, reset, sign, "", 0);
		ws_check_emit(line, len, ecbdata->ws_rule,
			      ecbdata->opt->file, set, reset, ws);
	emit_line_checked(reset, ecbdata, line, len,
			  DIFF_FILE_NEW, WSEH_NEW, '+');
	emit_line_checked(reset, ecbdata, line, len,
			  DIFF_FILE_OLD, WSEH_OLD, '-');
	emit_line_checked(reset, ecbdata, line, len,
			  DIFF_CONTEXT, WSEH_CONTEXT, ' ');
		emit_line(ecbdata->opt, context, reset, line, len);
	emit_line(ecbdata->opt, "", "", msgbuf.buf, msgbuf.len);
		if (is_tempfile_active(&diff_temp[i].tempfile))
static void print_line_count(FILE *file, int count)
		fprintf(file, "0,0");
		fprintf(file, "1");
		fprintf(file, "1,%d", count);
	static const char *nneof = " No newline at end of file\n";
	if (!endp) {
		const char *context = diff_get_color(ecb->color_diff,
						     DIFF_CONTEXT);
		putc('\n', ecb->opt->file);
		emit_line_0(ecb->opt, context, reset, '\\',
			    nneof, strlen(nneof));
	}
	const char *name_a_tab, *name_b_tab;
	const char *metainfo = diff_get_color(o->use_color, DIFF_METAINFO);
	const char *fraginfo = diff_get_color(o->use_color, DIFF_FRAGINFO);
	const char *reset = diff_get_color(o->use_color, DIFF_RESET);
	const char *line_prefix = diff_line_prefix(o);
	if (diff_mnemonic_prefix && DIFF_OPT_TST(o, REVERSE_DIFF)) {
	name_a_tab = strchr(name_a, ' ') ? "\t" : "";
	name_b_tab = strchr(name_b, ' ') ? "\t" : "";
	fprintf(o->file,
		"%s%s--- %s%s%s\n%s%s+++ %s%s%s\n%s%s@@ -",
		line_prefix, metainfo, a_name.buf, name_a_tab, reset,
		line_prefix, metainfo, b_name.buf, name_b_tab, reset,
		line_prefix, fraginfo);
		print_line_count(o->file, lc_a);
		fprintf(o->file, "?,?");
	fprintf(o->file, " +");
	print_line_count(o->file, lc_b);
	fprintf(o->file, " @@%s\n", reset);
static int fn_out_diff_words_write_helper(FILE *fp,
					  size_t count, const char *buf,
					  const char *line_prefix)
			fputs(line_prefix, fp);
			if (st_el->color && fputs(st_el->color, fp) < 0)
				return -1;
			if (fputs(st_el->prefix, fp) < 0 ||
			    fwrite(buf, p ? p - buf : count, 1, fp) != 1 ||
			    fputs(st_el->suffix, fp) < 0)
				return -1;
			if (st_el->color && *st_el->color
			    && fputs(GIT_COLOR_RESET, fp) < 0)
				return -1;
			return 0;
		if (fputs(newline, fp) < 0)
			return -1;
		fn_out_diff_words_write_helper(diff_words->opt->file,
				diff_words->current_plus, line_prefix);
		if (*(plus_begin - 1) == '\n')
			fputs(line_prefix, diff_words->opt->file);
		fn_out_diff_words_write_helper(diff_words->opt->file,
				minus_end - minus_begin, minus_begin,
				line_prefix);
		fn_out_diff_words_write_helper(diff_words->opt->file,
				plus_end - plus_begin, plus_begin,
				line_prefix);
		fputs(line_prefix, diff_words->opt->file);
		fn_out_diff_words_write_helper(diff_words->opt->file,
			diff_words->minus.text.ptr, line_prefix);
			fputs(line_prefix, diff_words->opt->file);
		fn_out_diff_words_write_helper(diff_words->opt->file,
			- diff_words->current_plus, diff_words->current_plus,
			line_prefix);
	if (ecb->truncate)
		return ecb->truncate(line, len);
	const char *meta = diff_get_color(ecbdata->color_diff, DIFF_METAINFO);
	const char *context = diff_get_color(ecbdata->color_diff, DIFF_CONTEXT);
	const char *line_prefix = diff_line_prefix(o);
		fprintf(o->file, "%s", ecbdata->header->buf);
		const char *name_a_tab, *name_b_tab;

		name_a_tab = strchr(ecbdata->label_path[0], ' ') ? "\t" : "";
		name_b_tab = strchr(ecbdata->label_path[1], ' ') ? "\t" : "";

		fprintf(o->file, "%s%s--- %s%s%s\n",
			line_prefix, meta, ecbdata->label_path[0], reset, name_a_tab);
		fprintf(o->file, "%s%s+++ %s%s%s\n",
			line_prefix, meta, ecbdata->label_path[1], reset, name_b_tab);
		if (line[len-1] != '\n')
			putc('\n', o->file);
		if (ecbdata->diff_words->type == DIFF_WORDS_PORCELAIN) {
			emit_line(o, context, reset, line, len);
			fputs("~\n", o->file);
		} else {
			/*
			 * Skip the prefix character, if any.  With
			 * diff_suppress_blank_empty, there may be
			 * none.
			 */
			if (line[0] != '\n') {
			      line++;
			      len--;
			}
			emit_line(o, context, reset, line, len);
		}
		emit_line(o, diff_get_color(ecbdata->color_diff, DIFF_CONTEXT),
			  reset, line, len);
static void show_name(FILE *file,
		      const char *prefix, const char *name, int len)
{
	fprintf(file, " %s%-*s |", prefix, len, name);
}

static void show_graph(FILE *file, char ch, int cnt, const char *set, const char *reset)
	fprintf(file, "%s", set);
	while (cnt--)
		putc(ch, file);
	fprintf(file, "%s", reset);
int print_stat_summary(FILE *fp, int files, int insertions, int deletions)
	int ret;
		return fprintf(fp, "%s\n", " 0 files changed");
	ret = fputs(sb.buf, fp);
	return ret;
	const char *line_prefix = "";
	line_prefix = diff_line_prefix(options);
			fprintf(options->file, "%s", line_prefix);
			show_name(options->file, prefix, name, len);
			fprintf(options->file, " %*s", number_width, "Bin");
				putc('\n', options->file);
			fprintf(options->file, " %s%"PRIuMAX"%s",
			fprintf(options->file, " -> ");
			fprintf(options->file, "%s%"PRIuMAX"%s",
			fprintf(options->file, " bytes");
			fprintf(options->file, "\n");
			fprintf(options->file, "%s", line_prefix);
			show_name(options->file, prefix, name, len);
			fprintf(options->file, " Unmerged\n");
		fprintf(options->file, "%s", line_prefix);
		show_name(options->file, prefix, name, len);
		fprintf(options->file, " %*"PRIuMAX"%s",
		show_graph(options->file, '+', add, add_c, reset);
		show_graph(options->file, '-', del, del_c, reset);
		fprintf(options->file, "\n");
			fprintf(options->file, "%s ...\n", line_prefix);
	fprintf(options->file, "%s", line_prefix);
	print_stat_summary(options->file, total_files, adds, dels);
		int deleted= data->files[i]->deleted;
	fprintf(options->file, "%s", diff_line_prefix(options));
	print_stat_summary(options->file, total_files, adds, dels);
	dir.cumulative = DIFF_OPT_TST(options, DIRSTAT_CUMULATIVE);
		if (DIFF_OPT_TST(options, DIRSTAT_BY_FILE)) {
	dir.cumulative = DIFF_OPT_TST(options, DIRSTAT_CUMULATIVE);
static void emit_binary_diff_body(FILE *file, mmfile_t *one, mmfile_t *two,
				  const char *prefix)
		fprintf(file, "%sdelta %lu\n", prefix, orig_size);
	}
	else {
		fprintf(file, "%sliteral %lu\n", prefix, two->size);
		char line[70];
		fprintf(file, "%s", prefix);
		fputs(line, file);
		fputc('\n', file);
	fprintf(file, "%s\n", prefix);
static void emit_binary_diff(FILE *file, mmfile_t *one, mmfile_t *two,
			     const char *prefix)
	fprintf(file, "%sGIT binary patch\n", prefix);
	emit_binary_diff_body(file, one, two, prefix);
	emit_binary_diff_body(file, two, one, prefix);
	if (DIFF_OPT_TST(o, REVERSE_DIFF)) {
		const char *del = diff_get_color_opt(o, DIFF_FILE_OLD);
		const char *add = diff_get_color_opt(o, DIFF_FILE_NEW);
		show_submodule_summary(o->file, one->path ? one->path : two->path,
				line_prefix,
				two->dirty_submodule,
				meta, del, add, reset);
		const char *del = diff_get_color_opt(o, DIFF_FILE_OLD);
		const char *add = diff_get_color_opt(o, DIFF_FILE_NEW);
		show_submodule_inline_diff(o->file, one->path ? one->path : two->path,
				line_prefix,
				two->dirty_submodule,
				meta, del, add, reset, o);
	if (DIFF_OPT_TST(o, ALLOW_TEXTCONV)) {
			fprintf(o->file, "%s", header.buf);
		fprintf(o->file, "%s", header.buf);
	} else if (!DIFF_OPT_TST(o, TEXT) &&
		    !DIFF_OPT_TST(o, BINARY)) {
					fprintf(o->file, "%s", header.buf);
			fprintf(o->file, "%s", header.buf);
			fprintf(o->file, "%sBinary files %s and %s differ\n",
				line_prefix, lbl[0], lbl[1]);
				fprintf(o->file, "%s", header.buf);
		fprintf(o->file, "%s", header.buf);
		if (DIFF_OPT_TST(o, BINARY))
			emit_binary_diff(o->file, &mf1, &mf2, line_prefix);
		else
			fprintf(o->file, "%sBinary files %s and %s differ\n",
				line_prefix, lbl[0], lbl[1]);
			fprintf(o->file, "%s", header.buf);
		if (DIFF_OPT_TST(o, FUNCCONTEXT))
		DIFF_OPT_SET(o, CHECK_FAILED);
			if (errno == ENOENT) {
			err_empty:
				err = -1;
			empty:
				s->data = (char *)"";
				s->size = 0;
				return err;
			}
	int fd;
	fd = mks_tempfile_ts(&temp->tempfile, template.buf, strlen(base) + 1);
	if (fd < 0)
	if (write_in_full(fd, blob, size) < 0)
	close_tempfile(&temp->tempfile);
	temp->name = get_tempfile_path(&temp->tempfile);
		int abbrev = DIFF_OPT_TST(o, FULL_INDEX) ? 40 : DEFAULT_ABBREV;
		if (DIFF_OPT_TST(o, BINARY)) {
	if (DIFF_OPT_TST(o, ALLOW_EXTERNAL)) {
			if (index_path(one->oid.hash, one->path, &st, 0))
	name  = p->one->path;
	other = (strcmp(name, p->two->path) ? p->two->path : NULL);
	if (!DIFF_OPT_TST(o, ALLOW_EXTERNAL))
	DIFF_OPT_SET(options, RENAME_EMPTY);
	if (DIFF_XDL_TST(options, IGNORE_WHITESPACE) ||
	    DIFF_XDL_TST(options, IGNORE_WHITESPACE_CHANGE) ||
	    DIFF_XDL_TST(options, IGNORE_WHITESPACE_AT_EOL))
		DIFF_OPT_SET(options, DIFF_FROM_CONTENTS);
		DIFF_OPT_CLR(options, DIFF_FROM_CONTENTS);
	if (DIFF_OPT_TST(options, FIND_COPIES_HARDER))
	if (!DIFF_OPT_TST(options, RELATIVE_NAME))
		DIFF_OPT_SET(options, RECURSIVE);
		DIFF_OPT_SET(options, RECURSIVE);
		DIFF_OPT_SET(options, DIRTY_SUBMODULES);
	if (DIFF_OPT_TST(options, QUICK)) {
		DIFF_OPT_SET(options, EXIT_WITH_STATUS);
	if (DIFF_OPT_TST(options, FOLLOW_RENAMES) && options->pathspec.nr != 1)
	else if (!strcmp(arg, "-X") || !strcmp(arg, "--dirstat"))
		return parse_dirstat_opt(options, "");
	else if (skip_prefix(arg, "-X", &arg))
		return parse_dirstat_opt(options, arg);
	else if (skip_prefix(arg, "--dirstat=", &arg))
	else if (!strcmp(arg, "--dirstat-by-file"))
		return parse_dirstat_opt(options, "files");
	else if (skip_prefix(arg, "--dirstat-by-file=", &arg)) {
	else if (starts_with(arg, "-B") || starts_with(arg, "--break-rewrites=") ||
		 !strcmp(arg, "--break-rewrites")) {
	else if (starts_with(arg, "-M") || starts_with(arg, "--find-renames=") ||
		 !strcmp(arg, "--find-renames")) {
	else if (starts_with(arg, "-C") || starts_with(arg, "--find-copies=") ||
		 !strcmp(arg, "--find-copies")) {
			DIFF_OPT_SET(options, FIND_COPIES_HARDER);
		DIFF_OPT_SET(options, RENAME_EMPTY);
		DIFF_OPT_CLR(options, RENAME_EMPTY);
	else if (!strcmp(arg, "--relative"))
		DIFF_OPT_SET(options, RELATIVE_NAME);
	else if (skip_prefix(arg, "--relative=", &arg)) {
		DIFF_OPT_SET(options, RELATIVE_NAME);
		options->prefix = arg;
	else if (!strcmp(arg, "--patience"))
	else if (!strcmp(arg, "--histogram"))
		DIFF_OPT_SET(options, BINARY);
		DIFF_OPT_SET(options, FULL_INDEX);
		DIFF_OPT_SET(options, TEXT);
		DIFF_OPT_SET(options, REVERSE_DIFF);
		DIFF_OPT_SET(options, FIND_COPIES_HARDER);
		DIFF_OPT_SET(options, FOLLOW_RENAMES);
		DIFF_OPT_CLR(options, FOLLOW_RENAMES);
		DIFF_OPT_CLR(options, DEFAULT_FOLLOW_RENAMES);
	} else if (!strcmp(arg, "--color"))
		options->use_color = 1;
	else if (skip_prefix(arg, "--color=", &arg)) {
	else if (!strcmp(arg, "--color-words")) {
	else if (skip_prefix(arg, "--color-words=", &arg)) {
		options->use_color = 1;
		options->word_diff = DIFF_WORDS_COLOR;
		options->word_regex = arg;
	}
		DIFF_OPT_SET(options, EXIT_WITH_STATUS);
		DIFF_OPT_SET(options, QUICK);
		DIFF_OPT_SET(options, ALLOW_EXTERNAL);
		DIFF_OPT_CLR(options, ALLOW_EXTERNAL);
	else if (!strcmp(arg, "--textconv"))
		DIFF_OPT_SET(options, ALLOW_TEXTCONV);
	else if (!strcmp(arg, "--no-textconv"))
		DIFF_OPT_CLR(options, ALLOW_TEXTCONV);
	else if (!strcmp(arg, "--ignore-submodules")) {
		DIFF_OPT_SET(options, OVERRIDE_SUBMODULE_CONFIG);
		handle_ignore_submodules_arg(options, "all");
	} else if (skip_prefix(arg, "--ignore-submodules=", &arg)) {
		DIFF_OPT_SET(options, OVERRIDE_SUBMODULE_CONFIG);
	} else if (!strcmp(arg, "--submodule"))
		options->submodule_format = DIFF_SUBMODULE_LOG;
	else if (skip_prefix(arg, "--submodule=", &arg))
		DIFF_OPT_SET(options, FUNCCONTEXT);
		DIFF_OPT_SET(options, FUNCCONTEXT);
		DIFF_OPT_CLR(options, FUNCCONTEXT);
	 * In well-behaved cases, where the abbbreviated result is the
static void show_file_mode_name(FILE *file, const char *newdelete, struct diff_filespec *fs)
		fprintf(file, " %s mode %06o ", newdelete, fs->mode);
		fprintf(file, " %s ", newdelete);
	write_name_quoted(fs->path, file, '\n');

static void show_mode_change(FILE *file, struct diff_filepair *p, int show_name,
		const char *line_prefix)
		fprintf(file, "%s mode change %06o => %06o%c", line_prefix, p->one->mode,
			p->two->mode, show_name ? ' ' : '\n');
			write_name_quoted(p->two->path, file, '\n');
static void show_rename_copy(FILE *file, const char *renamecopy, struct diff_filepair *p,
			const char *line_prefix)

	fprintf(file, " %s %s (%d%%)\n", renamecopy, names, similarity_index(p));
	show_mode_change(file, p, 0, line_prefix);
	FILE *file = opt->file;
	const char *line_prefix = diff_line_prefix(opt);

		fputs(line_prefix, file);
		show_file_mode_name(file, "delete", p->one);
		fputs(line_prefix, file);
		show_file_mode_name(file, "create", p->two);
		fputs(line_prefix, file);
		show_rename_copy(file, "copy", p, line_prefix);
		fputs(line_prefix, file);
		show_rename_copy(file, "rename", p, line_prefix);
			fprintf(file, "%s rewrite ", line_prefix);
			write_name_quoted(p->two->path, file, ' ');
			fprintf(file, "(%d%%)\n", similarity_index(p));
		show_mode_change(file, p, !p->score, line_prefix);
	if (0 < needed && needed < 32767)
	if (output_format & DIFF_FORMAT_DIRSTAT && DIFF_OPT_TST(options, DIRSTAT_BY_LINE))
	    DIFF_OPT_TST(options, EXIT_WITH_STATUS) &&
	    DIFF_OPT_TST(options, DIFF_FROM_CONTENTS)) {
			fprintf(options->file, "%s%c",
				diff_line_prefix(options),
				options->line_termination);
			if (options->stat_sep) {
				fputs(options->stat_sep, options->file);
			}
		for (i = 0; i < q->nr; i++) {
			struct diff_filepair *p = q->queue[i];
			if (check_pair_status(p))
				diff_flush_patch(p, options);
		}
	if (DIFF_OPT_TST(options, DIFF_FROM_CONTENTS)) {
			DIFF_OPT_SET(options, HAS_CHANGES);
			DIFF_OPT_CLR(options, HAS_CHANGES);
			if (!DIFF_OPT_TST(diffopt, NO_INDEX))
	if (diff_queued_diff.nr && !DIFF_OPT_TST(options, DIFF_FROM_CONTENTS))
		DIFF_OPT_SET(options, HAS_CHANGES);
		DIFF_OPT_CLR(options, HAS_CHANGES);
	if (!DIFF_OPT_TST(opt, EXIT_WITH_STATUS) &&
	if (DIFF_OPT_TST(opt, EXIT_WITH_STATUS) &&
	    DIFF_OPT_TST(opt, HAS_CHANGES))
	    DIFF_OPT_TST(opt, CHECK_FAILED))
	return (DIFF_OPT_TST(opt, QUICK) &&
		DIFF_OPT_TST(opt, HAS_CHANGES));
	unsigned orig_flags = options->flags;
	if (!DIFF_OPT_TST(options, OVERRIDE_SUBMODULE_CONFIG))
	if (DIFF_OPT_TST(options, IGNORE_SUBMODULES))
	if (DIFF_OPT_TST(options, REVERSE_DIFF))
	if (!DIFF_OPT_TST(options, DIFF_FROM_CONTENTS))
		DIFF_OPT_SET(options, HAS_CHANGES);
	if (DIFF_OPT_TST(options, REVERSE_DIFF)) {
	if (DIFF_OPT_TST(options, DIFF_FROM_CONTENTS))
	if (DIFF_OPT_TST(options, QUICK) && options->skip_stat_unmatch &&
	DIFF_OPT_SET(options, HAS_CHANGES);
	if (!DIFF_OPT_TST(opt, EXIT_WITH_STATUS) &&