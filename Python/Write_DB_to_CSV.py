import csv
import sqlite3
import os
# The default umask is 0o22 which turns off write permission of group and others

 
sqlite_file = '/home/odroid/Desktop/GregsDb/Python/DHT22.db'    # name of the sqlite database file
temp_table = 'DHT22_Temperature_Data'   # name of the table to be queried
hum_table = 'DHT22_Humidity_Data'   # name of the table to be queried
id_column = 'id'
column_2 = 'SensorID'
column_3 = 'Date_n_Time'
column_4_t = 'Temperature'
column_4_h = 'Humidity'

def CSV_export():
	# Connecting to the database file
	conn = sqlite3.connect(sqlite_file)
	t = conn.cursor()
	h = conn.cursor()

	t.execute('SELECT * FROM {tn}'.\
			 format(tn=temp_table))
	h.execute('SELECT * FROM {tn}'.\
			 format(tn=hum_table)) 

	Temp = t.fetchall()
	Hum = h.fetchall()
        
	with open("/var/www/html/DHT22_temperatures.csv", "wb") as csv_file:              # Python 2 version
		csv_writer = csv.writer(csv_file)
		#csv_writer.writerow([i[0] for i in t.description]) # write headers
		csv_writer.writerows(Temp)
        os.chmod("/var/www/html/DHT22_temperatures.csv", 0o777)
		
	with open("/var/www/html/DHT22_humidities.csv", "wb") as csv_file:              # Python 2 version
		csv_writer = csv.writer(csv_file)
		#csv_writer.writerow([i[0] for i in h.description]) # write headers
		csv_writer.writerows(Hum)
        os.chmod("/var/www/html/DHT22_humidities.csv", 0o777)

	conn.close()