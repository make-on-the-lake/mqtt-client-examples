#!/usr/bin/env ruby

require 'mqtt'
require 'json'


HOST = 'otis.leandog.com'
PORT = 52122
USERNAME = 'meetup'
#PASSWORD = '<replace_with_password>'
PASSWORD = 'H1uJ6FUK4ELx'

#UID = '<replace_with_your_name_or_handle>'
UID = 'garygj'
ONLINE_TOPIC = "/boat/meetup/#{UID}/online"
STATUS_TOPIC = "/boat/meetup/#{UID}/status"


client = MQTT::Client.new
client.host = HOST
client.port = PORT
client.username = USERNAME
client.password = PASSWORD
client.client_id = UID

# Set a will message stating that we are offline
client.will_topic = ONLINE_TOPIC
client.will_payload = '0'
client.will_qos = 1
client.will_retain = true

puts 'connecting to mqtt broker...'
client.connect()
puts 'connected to mqtt broker'

# Publish a message stating that we are online
client.publish(ONLINE_TOPIC, '1', retain=true)

# Publish our device status
epoch_time = Time.now.to_i
message = "#{UID} is sending messages from ruby!"
payload = {'statusMessage': message, 'lastUpdated': epoch_time}
payload_json = payload.to_json
client.publish(STATUS_TOPIC, payload_json, retain=true)

# Subscribe to device statuses
client.subscribe('/boat/meetup/#')

client.get do |topic,message|
  puts "Got topic \"#{topic}\" with payload \"#{message}\""
end
