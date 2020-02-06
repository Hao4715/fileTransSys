main : main.o get_php_oper_info.o mission.o p2p_download.o p2p_continue.o p2p_server.o http17.o ftp.o ftp_download.o
	gcc main.o get_php_oper_info.o mission.o p2p_download.o p2p_continue.o p2p_server.o http17.o ftp.o ftp_download.o -lcrypto -lssl -o main -lpthread -g
main.o : main.c get_php_oper_info.h mission.h
	gcc main.c -c
get_php_oper_info.o : get_php_oper_info.c get_php_oper_info.h
	gcc get_php_oper_info.c -c
mission.o : mission.c mission.h p2p_download.h get_php_oper_info.h p2p_continue.h http17.h
	gcc mission.c -c
p2p_download.o : p2p_download.c p2p_download.h get_php_oper_info.h
	gcc p2p_download.c -c 
p2p_continue.o : p2p_continue.c p2p_continue.h get_php_oper_info.h p2p_download.h
	gcc p2p_continue.c -c
p2p_server.o : p2p_server.c p2p_server.h
	gcc p2p_server.c -c
http17.o : http17.c http17.h get_php_oper_info.h
	gcc http17.c -c 
ftp.o : ftp.c ftp.h
	gcc ftp.c -c
ftp_download.o : ftp_download.c ftp_download.h ftp.h
	gcc ftp_download.c -c
#clean:
#rm main.o get_php_oper_info.o mission.o p2p_download.o
