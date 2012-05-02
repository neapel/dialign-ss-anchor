#!/usr/bin/env python

def read_matrix(lines):
	'''
	Reads a 2D matrix with row/column titles.
	Yields (row, column, value) tuples.
	Supports row titles on the left (lower triangle matrix)
	or on the right (upper triangle matrix)
	Non-triangle forms and mixtures also supported.
	Missing columns not supported.
	'''
	columns = None
	columns_i = None
	for line in lines:
		line = line.strip()
		if len(line) > 0 and line[0] != '#':
			if not columns:
				# first row: column titles.
				columns = line.split()
				columns_i = iter(columns)
			else:
				# next rows: data, with row titles.
				cells = line.split()
				if cells[0] in columns:
					# Row title on left.
					# Missing values on the right.
					row = cells[0]
					content = cells[1:]
					for column, value in zip(columns, content):
						yield row, column, value
				elif cells[-1] in columns:
					# Row title on right.
					# Missing values on the left.
					row = cells[-1]
					content = cells[:-1]
					for column, value in zip(reversed(columns),reversed(content)):
						yield row, column, value
				else:
					# No row title, assume column order.
					# Missing values to the right.
					row = next(columns_i)
					for column, value in zip(columns, cells):
						yield row, column, value

def mat_dict(items):
	'''
	Return column names and dictionary (row, col)->value
	'''
	columns = set()
	data = {}
	for row, column, value in items:
		data[(row, column)] = value
		columns.add(row)
		columns.add(column)
	return (columns, data)

def full_form(items):
	'''
	For missing cells, assume m(i,j) = m(j,i)
	'''
	columns, data = mat_dict(items)
	# set missing:
	for row in columns:
		for col in columns:
			if (row,col) not in data and (col,row) in data:
				data[(row,col)] = data[(col,row)]
	return columns, data



def read_simple(lines):
	''' read cells '''
	for line in lines:
		yield line.split()

def compact(columns, data):
	''' create aligned compact string '''
	columns = list(sorted(columns))
	rows = []
	rows.append([''] + columns)
	for row in columns:
		rows.append([row] + list([
			data[(row, col)] if (row, col) in data else ''
			for col in columns]))
	width = max([len(col) for row in rows for col in row])
	return '\n'.join([
		' '.join([col.rjust(width) for col in row])
		for row in rows
	])

if __name__ == '__main__':
	import argparse, sys
	parser = argparse.ArgumentParser(description='Convert a compact matrix format into simple to parse triple format.')
	group = parser.add_mutually_exclusive_group(required=True)
	group.add_argument('--compact', action='store_true', help='Convert simple input to compact matrix format')
	group.add_argument('--simple', action='store_true', help='Convert compact input to simple list format')
	parser.add_argument('input', nargs='?', type=argparse.FileType('r'), default=sys.stdin, help='The input file, leave for stdin')
	parser.add_argument('output', nargs='?', type=argparse.FileType('w'), default=sys.stdout, help='The output file, leave for stdout')
	args = parser.parse_args()

	if args.simple:
		for (row, column), value in full_form(read_matrix(args.input))[1].items():
			print >>args.output, row, column, value
	else:
		print >>args.output, compact(*full_form(read_simple(args.input)))
