ir-llvm:
	clang -emit-llvm -S main.c -o main.ll
	cland -emit-llvm -S sylib,c -o sylib,ll

exe:
	clang main.ll sylib.c -o out

.PHONY: clean

clean:
	remain main.ll
