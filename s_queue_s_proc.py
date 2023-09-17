#!/usr/bin/env python3
import simpy
from server_sim import server
import matplotlib.pyplot as plot
from random_gen import random_gen

if __name__ == "__main__":

    num_req = 10
    
    for i in range(20):
        random_obj = random_gen()

        start_time, runtime, queue, resp_list = random_obj.gen_requests(5, num_req, int(0.1*num_req), [3,20],[200,1000])

        serv = server(queue, resp_list)    
    
        serv.serve()
        print("####################################################################")    
        print("### Results ###")
        print("Arrival Rate : {}".format(serv.arrival_rate))
        print("Server Utilisation: {} ".format(serv.server_util))
        print("Server Average Response: {}".format(serv.server_avg_resp))
        print("Server Standard Deviation Response: {}".format(serv.server_std_resp))
        print("####################################################################")