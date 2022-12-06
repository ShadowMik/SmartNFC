import pymysql
import sys

# Forbind t. database via ubuntu: mysql -h <host> -P <port> -u <master_username> -p
# Database hedder iotproject, tabel hedder users. tre kolonner: ID (tæller auto op fra 1), nfcid, navn

# Denne lambda-funktion tager imod et JSON-objekt (kaldet event), og sammenligner indholdet m. indholdet i en database, 
# for at beslutte om en bruger er registreret eller ej (har rettigheder til at låse / låse op).
# Objektet der modtages indeholder et NFC ID, fra den nøglebrik/kort der er scannet via IoT-enheden.
# Returnerer et "ja, brugeren er registreret i databasen", eller "Nej, brugeren er ikke registreret".

# TODO:     


rds_host = 'database-iotproject.cqzxkipyh6vw.us-east-2.rds.amazonaws.com'
rds_port = 3306
username = 'admin'
password = 'password'
db_name = 'iotproject'
tb_name = 'users'

connection = pymysql.connect(host=rds_host,
                           port=rds_port,
                           user=username,
                           passwd=password,
                           db=db_name)


def searchForID(request):
#    with connection:
    with connection.cursor() as cursor:
        cursor.execute(f"SELECT nfcid FROM {tb_name} WHERE nfcID={request['nfcid']}")
        connection.commit()
        if cursor.rowcount != 0:
            return 1
        else:
            return 0


def lambda_handler(event, context):
    result = searchForID(event)
    if result == 1:
        return "True"
    else:
        return "False"
