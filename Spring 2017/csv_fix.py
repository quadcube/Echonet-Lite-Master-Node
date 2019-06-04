import csv
import datetime
from dateutil import parser

row_counter = 0
csv_timestamp = None

with open("eetcclog.csv", 'r') as csv_file1, open("eetcclog_fixed.csv", 'w') as csv_file2:
    csv_obj1 = csv.reader(csv_file1, delimiter=',')
    csv_obj2 = csv.writer(csv_file2, delimiter=',')
    for csv_row in csv_obj1:
        row_counter += 1
        if row_counter == 10:
            csv_timestamp = parser.parse(csv_row[0]) # eetcclog.csv 1th col is the initial timestamp
            csv_obj2.writerow(csv_row)
        elif row_counter < 15:
            csv_obj2.writerow(csv_row)
        elif len(csv_row) > 0:
            if csv_row[0].isdigit(): # eetcclog.csv 1st col is an index
                csv_timestamp = csv_timestamp + datetime.timedelta(seconds=10)
                csv_row[55] = str(csv_timestamp) # eetcclog.csv 56th col is timestamp
                csv_obj2.writerow(csv_row)
