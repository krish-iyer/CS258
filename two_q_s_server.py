#!/usr/bin/env python3
from server_sim_two_q import server
import matplotlib.pyplot as plot
from random_gen import random_gen

if __name__ == "__main__":

    num_req = 100
    
    # for i in range(20):
    random_obj = random_gen()

    start_time, runtime, queue, resp_list = random_obj.gen_requests(5, num_req, int(0.1*num_req), [3,20],[200,1000])

    proc = 20
    serv = server(queue, resp_list, proc)    

    serv.serve()
    print("####################################################################")    
    print("### Results ###")
    print("Arrival Rate : Long {} Short {}".format(serv.arrival_rate_long, serv.arrival_rate))
    print("Server Utilisation: Long {} Short {}".format(serv.server_util_long, serv.server_util))
    print("Server Average Response: Long {} Short {}".format(serv.server_avg_resp_b,serv.server_avg_resp_a))
    print("Server Standard Deviation Response: {}".format(serv.server_std_resp))
    print("####################################################################")