import pymysql
import sys
import datetime

# Forbind t. database via ubuntu: mysql -h <host> -P <port> -u <master_username> -p

# Opret tabeller hvis database er genstartet:
# create table users (id INT AUTO_INCREMENt PRIMARY KEY, nfcID VARCHAR(255), name VARCHAR(255));
# create table userlog (event_id INT AUTO_INCREMENT PRIMARY KEY, user VARCHAR(255), datetime VARCHAR(255), doorstate VARCHAR(255));
# Format for indsætning i users: insert into users (nfcid, name) values (22334456, "Anders");

# Database hedder iotproject, tabel hedder userlog. Fire kolonner: ID (tæller auto op fra 1), user, datetime og doorstate

# Denne lambda-funktion tager imod et JSON-objekt (kaldet event), og indsætter dataen heri, i en logging-tabel i en database.
# Objektet der modtages indeholder et NFC ID, fra den nøglebrik/kort der er scannet via IoT-enheden, samt en tilstand på døren (låst/ikke-låst)

# TODO:     


rds_host = 'database-iotproject.cqzxkipyh6vw.us-east-2.rds.amazonaws.com'
rds_port = 3306
username = 'admin'
password = 'password'
db_name = 'iotproject'
tb1_name = 'userlog'
tb2_name = 'users'

connection = pymysql.connect(host=rds_host,
                           port=rds_port,
                           user=username,
                           passwd=password,
                           db=db_name)


def save_data(data):
    with connection.cursor() as cursor:
        cursor.execute(f"SELECT name FROM {tb2_name} WHERE nfcID={data['nfcid']}")
        connection.commit()
        result = cursor.fetchone()
        
        now = datetime.datetime.utcnow()
        
        if result is None:
            sql1 = f"INSERT INTO {tb2_name} (nfcID) VALUES (%s)"
            cursor.execute(sql1, ({data['nfcid']}))
            sql2 = f"INSERT INTO {tb1_name} (user, datetime, doorstate) VALUES (%s, %s, %s)"
            cursor.execute(sql2, ({data['nfcid']}, now, {data['doorstate']}))
            connection.commit()
        else:
            sql = f"INSERT INTO {tb1_name} (user, datetime, doorstate) VALUES (%s, %s, %s)"
            cursor.execute(sql, ({data['nfcid']}, now, {data['doorstate']}))
            connection.commit()


def lambda_handler(event, context):
    save_data(event)
    return "User access logged in database"
