#!/usr/bin/env python3
import sys

sys.path.append("../")

import logging

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    handlers=[
        logging.FileHandler("../logs/two_queue_s_proc.log"),
        logging.StreamHandler()
    ]
)

from server_sim_two_q import server
from random_gen import random_gen

if __name__ == "__main__":

    num_req = 100
    
    rs_arrival_rate_arr = []
    rs_server_util_arr = []
    rs_server_avg_resp_arr = [] 
    rs_server_std_resp_arr = []
    
    for i in range(20):
        
        proc = 20
        random_obj = random_gen()

        start_time, runtime, queue, resp_list = random_obj.gen_requests(5, num_req, int(0.1*num_req), [3,20],[200,1000])

        
        serv = server(queue, resp_list, proc)    

        serv.serve()
        rs_arrival_rate_arr.append(serv.arrival_rate)
        rs_server_util_arr.append(serv.server_util)
        rs_server_avg_resp_arr.append(serv.server_avg_resp)
        rs_server_std_resp_arr.append(serv.server_std_resp)
        
        logging.info("### Results {} : proc: {} ###".format(i, proc))
        logging.info("Arrival Rate : {} ; Server Utilisation : {} ; Server Average Response : {} ; Server Standard Deviation Response : {}".format(serv.arrival_rate, \
                serv.server_util, serv.server_avg_resp, serv.server_std_resp))

    logging.info("## Final ##")
    logging.info("Avg Arrival Rate : {} ; Avg Server Utilisation : {} ; Avg Server Average Response : {} ; Avg Server Standard Deviation Response : {}".format(random_gen.mean(serv.arrival_rate), \
                random_gen.mean(serv.server_util), random_gen.mean(serv.server_avg_resp), random_gen.mean(serv.server_std_resp)))
    