# pmsg

pmsg is a simple linux based implementation of a tcp message broker,
it uses a FIFO data structure to retain all of the data,
and uses epoll for multiple async connections

## protocol

pmsg uses a text based protocol on top of tcp

#### ex
```txt
PUSH <room-name> <data for that room>
OK message saved

POP <room-name>
DATA <data for that room>
```

## usage
to connect to pmsg you can easily implement a client
with a few lines of code. check es_client.py.
or you can simply use using something like netcat
and manually type the commands

#### ex
```txt
nc 127.0.0.1 5555
PUSH <room-name> ...
```



## License

Released under the GPLv3 (or later) License.
