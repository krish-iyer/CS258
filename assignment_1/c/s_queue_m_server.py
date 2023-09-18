#!/usr/bin/env python3
import sys

sys.path.append("../b/")
sys.path.append("../")

from server_sim import server
from random_gen import random_gen

if __name__ == "__main__":

    num_req = 1000
    
    random_obj = random_gen()

    start_time, runtime, queue, resp_list = random_obj.gen_requests(5, num_req, int(0.1*num_req), [3,20],[200,1000])

    proc = 20
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