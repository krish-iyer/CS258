#!/usr/bin/env python3
from server_sim_rr_q import server
from random_gen import random_gen

if __name__ == "__main__":

    num_req = 100
    
    random_obj = random_gen()

    start_time, runtime, queue, resp_list = random_obj.gen_requests(5, num_req, int(0.1*num_req), [3,20],[200,1000])

    proc = 2
    serv = server(queue, resp_list, proc)    

    serv.serve()
    print("####################################################################")    
    print("### Results ###")
    print("End time: {}".format(serv.end_time))
    print("Arrival Rate : {}".format(serv.arrival_rate))
    print("Server Utilisation: {} ".format(serv.server_util))
    print("Server Average Response: {}".format(serv.server_avg_resp))
    print("Server Standard Deviation Response: {}".format(serv.server_std_resp))
    print("####################################################################")