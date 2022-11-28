import pymysql
import sys

# Forbind t. database via ubuntu: mysql -h <host> -P <port> -u <master_username> -p
# Database hedder iotproject, tabel hedder userlog. Fire kolonner: ID (tæller auto op fra 1), user, datetime og doorstate
# Next step: implementering af API så webhook fra particle-miljø kan sende datapakke (JSON-format) til denne lambdafunktion

rds_host = 'database-iotproject.cqzxkipyh6vw.us-east-2.rds.amazonaws.com'
rds_port = 3306
username = 'admin'
password = 'password'
db_name = 'iotproject'
tb_name = 'userlog'

connection = pymysql.connect(host=rds_host,
                           port=rds_port,
                           user=username,
                           passwd=password,
                           db=db_name)


def save_data(data):
    with connection:
        with connection.cursor() as cursor:
            sql = f"INSERT INTO {tb_name} (user, datetime, doorstate) VALUES (%s, %s, %s)"
            cursor.execute(sql, ({data['user']}, {data['datetime']}, {data['doorstate']}))
            connection.commit()


def read_data():
    result = []
    with connection:
        with connection.cursor() as cursor:
            cursor.execute(f"SELECT * FROM {tb_name}")
            connection.commit()
            for row in cursor:
                result.append(list(row))
            print("Currently stored data:\n")
            print(result)


def lambda_handler(event, context):
    # implement: if event is X = save data, if event is Y, return currently stored data
    save_data(event)
    return


#package = {"user": "Anders", "datetime": "28-11-2022 14:04:35", "doorstate": "locked"}
#save_data(package)
#read_data()
