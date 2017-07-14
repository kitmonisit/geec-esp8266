import itertools

def cleanup_group(group_member):
    if group_member: return group_member[:-1]

def generate_query(group):
    group = list(itertools.compress(map(cleanup_group, group), group))
    ss0 = "\nmisc_editArr(query_array, "
    ss1 = '''"{}", '''*len(group)
    ss2 = "sizeof query_array, element_size);"
    ss = ''.join(
            ["stream_begin();",
             ss0, ss1, ss2,
             "stream_query(query_array);",
             "\nstream_end();"]).format(*group)
    return ss

def grouper(iterable, n, fillvalue=None):
    args = [iter(iterable)] * n
    return itertools.izip_longest(fillvalue=fillvalue, *args)

with open('queries', 'r') as fd:
    g = grouper(fd, 5)
    while True:
        try:
            print generate_query(g.next())
        except StopIteration: break

