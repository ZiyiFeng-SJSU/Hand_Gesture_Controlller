#!/usr/bin/python

from __future__ import print_function

import os
import sys
import errno
import dbus
import dbus.service
import dbus.mainloop.glib
from xml.etree import ElementTree

FIFO = '/tmp/gesturepipe'

if __name__ == '__main__':
    bus = dbus.SystemBus()
    path = '/org/bluez'
    obj = bus.get_object('org.bluez', path)
    iface = dbus.Interface(obj, 'org.freedesktop.DBus.Introspectable')
    xml_string = iface.Introspect()
    for child in ElementTree.fromstring(xml_string):
        if child.tag == 'node':
            if 'hci' in child.attrib['name']:
                path = '/'.join((path, child.attrib['name']))
                break
    obj = bus.get_object('org.bluez', path)
    iface = dbus.Interface(obj, 'org.freedesktop.DBus.Introspectable')
    xml_string = iface.Introspect()
    media_path = 'UNDEFINED'
    for child in ElementTree.fromstring(xml_string):
        if child.tag == 'node':
            new_path = '/'.join((path, child.attrib['name']))
            obj = bus.get_object('org.bluez', new_path)
            iface = dbus.Interface(obj, 'org.freedesktop.DBus.Introspectable')
            xml_string = iface.Introspect()
            for child in ElementTree.fromstring(xml_string):
                if child.tag == 'node':
                    if 'player' in child.attrib['name']:
                        media_path = '/'.join((new_path, child.attrib['name']))
                        break
            if media_path != 'UNDEFINED':
                break
    if media_path == 'UNDEFINED':
        print("Error: Media device not connected")
        sys.exit()
    media = dbus.Interface(bus.get_object('org.bluez', media_path), 'org.bluez.MediaPlayer1')

    try:
        os.mkfifo(FIFO)
    except OSError as oe:
        if oe.errno == errno.EEXIST:
            print("Pipe Already Exists")
        if oe.errno != errno.EEXIST:
            print("Error opening pipe")
            raise

    print("Pipe Created")

    try:
        fifo = open(FIFO, "r", 0)
    except OSError as oe:
            raise
    print("Pipe Opened")

    while True:
        data = fifo.read(1)
        if len(data) == 0:
            print("Pipe Closed")
            break
        print('Read: "{0}"'.format(data))
        if(data == "0"):
            media.FastForward()
            print("FastForward")
        elif(data == "1"):
            media.Next()
            print("Next")
        elif(data == "2"):
            media.Pause()
            print("Pause")
        elif(data == "3"):
            media.Play()
            print("Play")
        elif(data == "4"):
            media.Previous()
            print("Previous")
        elif(data == "5"):
            media.Rewind()
            print("Rewind")
        elif(data == "6"):
            media.Stop()
            print("Stop")
        #elif(data == "7"):
            #media.VolumeDown()
            #print("VolumeDown")
        #elif(data == "8"):
            #media.VolumeUp()
            #print("VolumeUp")


