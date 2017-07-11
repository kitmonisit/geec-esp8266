def generate_query(line):
    return 'stream_add_query_response_pair(root, "{0:s}");'.format(line[:-1])

with open('queries', 'r') as fd:
    out = map(generate_query, fd)

for o in out:
    print o

