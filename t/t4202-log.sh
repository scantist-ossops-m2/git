	git log --pretty="tformat:%s" --invert-grep --grep=th --grep=Sec >actual &&
	test_cmp expect actual
	git log --pretty="tformat:%s" --invert-grep -i --grep=th --grep=Sec >actual &&
	test_cmp expect actual
test_expect_success 'log -F -E --grep=<ere> uses ere' '
	git log -1 --pretty="tformat:%s" -F -E --grep=s.c.nd >actual &&
| * commit side
	git log --oneline >expect.none &&
test_expect_success GPG 'log --graph --show-signature' '
	git commit -S -m signed_commit &&