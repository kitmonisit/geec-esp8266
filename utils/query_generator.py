def generate_query(line):
    return 'stream_add_query_response_pair(root, "{0:s}");'.format(line[:-1])

with open('queries', 'r') as fd:
    out = map(generate_query, fd)

k = 0
for n, o in enumerate(out):
    print o
    if (n+1) % 5 == 0:
        print k + 1
        k += 1

ss = "stream_begin();\nstream_query_{0:02d}();\nstream_end();"
for n in range(28):
    print ss.format(n)
