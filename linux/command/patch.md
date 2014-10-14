
step one:
  file a.md
	test1
	test2
	test3

  file b.md
	test4
	test5
	test2

step two:
	diff -uN a.md b.md >ab.patch

step three:
	patch -p0 < ab.patch

step four:
	patch -RE -p0 < ab.patch

done.

