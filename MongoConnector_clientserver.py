###############################################################################
# Standalone Python program acting as the edge to receive and process XINU UDP datagrams and insert them in MONGO DB and to send UDP datagram to XINU regarding state of LED(on or off) while inserting into MONGO.
###############################################################################
import sys
import pymongo
from pymongo import MongoClient
import datetime
import json
import jsonschema
from jsonschema import validate
import socket
import datetime
import time

# server connection

### Standard URI format: mongodb://[dbuser:dbpassword@]host:port/dbname

MONGODB_URI = 'mongodb://ramonavaz:ramonavaz25@ds147777.mlab.com:47777/dosproject'
MESSAGE = ''
client = pymongo.MongoClient(MONGODB_URI)
# Note that the insert method can take either an array or a single dict.

current_datetime = datetime.datetime.now()
print(current_datetime)

# data connections



###############################################################################
# Schema Definition
###############################################################################

SCHEMA = {
    "temperature": {
        "type": "string"
    },
    "system_power": {
        "type": "string"
    },
    "led_status": {
        "type": "string"
    },
    "timestamp": {
        "type": "string"
    }

}

#SEED_DATA = {"temperature": "35","system_power": "ON","led_status": "1","timestamp":current_datetime.isoformat()}


###############################################################################
# main
###############################################################################
def main(args):
    
    #background process to insert data keeps running
    
    #retrieve last record from mongo db
    while True:
     
     #send XINU the latest LED status
     sendToXinu(retrieveLatest())
     time.sleep(5)
     receivedFromXinu()
     time.sleep(1)
     #client.close()
   


###############################################################################
# Validate JSON
###############################################################################
def validateJson(SEED_DATA):
    try:
       validate(SEED_DATA,SCHEMA)
    except ValueError:
         return False
    return True

###############################################################################
# Use a mongo connector and write valid JSON to database
###############################################################################
def writeToDatabase(SEED_DATA):   
    # Our database collection is called sensordata
    db = client.get_default_database()
    print(db)
    #print("string",list(SEED_DATA.decode()))
    data_insert = json.loads(SEED_DATA.decode())
    print("json",data_insert)
    collection = db['sensordata']
    print(collection) 
    collection.insert(data_insert)

###############################################################################
# Retrieve latest LED status from Mongo DB
###############################################################################
def retrieveLatest():
    # Our database collection is called sensordata
    db = client.get_default_database()
    results = list(db['sensordata'].find({}).sort([("led_status",-1)]).limit(1))
    print(results)      
    # display documents from collection
    for record in results:
        # print out the document
        print(record['temperature'] + ',',record['led_status'])
        MESSAGE = record['led_status']
        print(MESSAGE.encode('utf8'))
        return MESSAGE

###############################################################################
# Server to send data from XINU,(LED on or off)
###############################################################################
def sendToXinu(data):
    print(data)
    UDP_IP = "192.168.1.37"
    UDP_PORT = 5000
    sock_send= socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock_send.sendto(data.encode(),(UDP_IP, UDP_PORT))
    
###############################################################################
# Client to receive data from XINU
###############################################################################
def receivedFromXinu():
    UDP_IP = ""
    UDP_PORT = 3000
    sock_rec = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock_rec.bind(("", UDP_PORT))
    print("receive");
    data, addr = sock_rec.recvfrom(4096) # buffer size is 4096 bytes  
    print("received message:", data.decode())
    #if (validateJson(data)):
    # print("Writing to Database")
    writeToDatabase(data)
    #else:
    #  print("Unsuccessful in writing to Database") 


if __name__ == '__main__':
    main(sys.argv[1:])
