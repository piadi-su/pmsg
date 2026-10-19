import readline
import socket

PORT = 5555
HOST = "127.0.0.1"

def PUSH(s):
    room = input("enter Room to PUSH in: ")
    data = input(f"enter date to push in {room}: ")
    
    payload = f"PUSH {room} {data}"

    s.send(payload.encode())
    pmsg_data: bytes = s.recv(4096)

    print(f"Recived response:\n{pmsg_data}")

def POP(s):
    room = input("enter Room to POP in: ")
    
    payload = f"POP {room}"

    s.send(payload.encode())
    pmsg_data: bytes = s.recv(4096)

    print(f"Recived response:\n{pmsg_data}")





    


def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    s.connect((HOST, PORT))
    
    print("[+] connected to pmsg")

    while True:

        print("options: 1. PUSH, 2. POP")
        opt = int(input("> "))

        if opt != 1 and opt != 2:
            continue

        elif opt == 1:
            PUSH(s)
        elif opt == 2:
            POP(s)

        print("\n")


        
        

    


if __name__ == "__main__":
    main()
