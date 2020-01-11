sem_to_PC:sem_to_ProCon.cpp
		g++ $^ -o $@ -lpthread
pro_con:pro_con.cpp
		g++ $^ -o $@ -lpthread
shengji:shengji.c
		gcc $^ -o $@ -lpthread
cond:cond.c
		gcc $^ -o $@ -lpthread
