# pmsg

pmsg is a simple linux based implementation of a tcp message broker,
it uses a FIFO data structior to retain all of the data,
and uses epoll for multiple async connections

## protocol

pmsg uses a text based protocoll on top of tcp

#### es
```txt
PUSH <room-name> <data for that room>
OK message saved

POP <room-name>
DATA <data for that room>
```

## usage
to connect to pmsg you can easily realize a client
with few lines of code. check es_client.py.
or it's as simple as using something like netcat
and manually write the commands

#### es
```txt
nc 127.0.0.1 5555
PUSH <room-name> ...
```



## License

Released under the GPLv3 (or later) License.
