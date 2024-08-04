import glob

c_sources = glob.glob('**/*.c')
h_sources = glob.glob('**/*.h')
s_sources = glob.glob('**/*.s')

sources = c_sources + h_sources + s_sources

total_lines = 0
for i in sources:
    total_lines += sum(1 for _ in open(i))

print(total_lines)
