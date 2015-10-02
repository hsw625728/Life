gmsv : main.o init.o log.o resource.o dbsv_proto.o db.o dbsvproto_cli.o util.o net.o account.o gmsv_proto.o gmsvproto_sv.o account_cli.o account_dbsv.o net_packet.o
	gcc -o gmsv -g  main.o init.o log.o resource.o dbsv_proto.o db.o dbsvproto_cli.o util.o gmsv_proto.o gmsvproto_sv.o account.o account_cli.o account_dbsv.o -L/usr/lib64/mysql -lmysqlclient net.o -levent net_packet.o -lpthread
main.o : main.c system.h type.h
	gcc -c main.c -g
db.o : db.c system.h type.h /usr/include/mysql/mysql.h
	gcc -c db.c /usr/include/mysql/mysql.h -g
init.o : init.c init.h util.h system.h type.h
	gcc -c init.c -g
log.o : log.c log.h system.h type.h
	gcc -c log.c -g
resource.o : resource.c resource.h system.h type.h
	gcc -c resource.c -g
dbsv_proto.o : dbsv_proto.h system.h type.h
	gcc -c dbsv_proto.c -g
dbsvproto_cli.o : dbsvproto_cli.h system.h type.h
	gcc -c dbsvproto_cli.c -g
util.o : util.c system.h type.h
	gcc -c util.c -g
net.o : net.c net.h system.h type.h dbsv_proto.h
	gcc -c net.c dbsv_proto.h -levent -g
account.o : ./account/account.c ./account/account.h system.h
	gcc -c ./account/account.c ./account/account.h -g
gmsv_proto.o : gmsv_proto.c gmsv_proto.h system.h ./account/account.h
	gcc -c gmsv_proto.c gmsv_proto.h ./account/account.h -g
gmsvproto_sv.o : gmsvproto_sv.c gmsvproto_sv.h system.h 
	gcc -c gmsvproto_sv.c gmsvproto_sv.h -levent -g
account_cli.o : ./account/account_cli.c ./account/account.h system.h
	gcc -c ./account/account_cli.c ./account/account.h system.h -g
account_dbsv.o : ./account/account_dbsv.c ./account/account.h system.h
	gcc -c ./account/account_dbsv.c ./account/account.h system.h -g
net_packet.o : net_packet.c
	gcc -c net_packet.c net_packet.h -g
clean :
	rm gmsv main.o init.o log.o resource.o net.o dbsv_proto.o dbsvproto_cli.o util.o gmsv_proto.o gmsvproto_sv.o *.gch account_cli.o account.o account_dbsv.o db.o net_packet.o
