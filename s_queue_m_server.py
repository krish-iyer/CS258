#!/usr/bin/env python3
import simpy
from sim_server_ex import server
from random_gen import random_gen

if __name__ == "__main__":

    num_req = 10
    
    random_obj = random_gen()

    start_time, runtime, queue, queue_a, queue_b, resp_list_a, resp_list_b = random_obj.gen_requests(5, num_req, int(0.1*num_req), [3,20],[200,1000])

    a = [1,2,3,4,5]
    env = simpy.Environment(start_time)
    proc = simpy.Resource(env, capacity=2)
    serv = server(a, env, proc, queue_a, queue_b, resp_list_a, resp_list_b)    
    env.run(runtime)
    print("runtime: {}".format(runtime)) 
    print("####################################################################")    
    print("### Results ###")
    print("Arrival Rate : {}".format(serv.arrival_rate))
    print("Server Utilisation: {} ".format(serv.server_util))
    print("Server Average Response: {}".format(serv.server_avg_resp))
    print("Server Standard Deviation Response: {}".format(serv.server_std_resp))
    print("####################################################################")