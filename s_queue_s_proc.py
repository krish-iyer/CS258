#!/usr/bin/env python3
import simpy
from random_gen import random_gen
import matplotlib.pyplot as plot

'''
TODO:
    1. Create two queues for two kinds of processes with 90% and 10% of
        of the total requests
    
    2. With each request, create an unique ID.
    3. Write the response time in the list with the arrival time
    4. Handle idle time
DONE:    
'''

class server():
    def __init__(self, env, que_a, que_b, resp_a, resp_b):
        self.env = env
        self.action = env.process(self.run(que_a, que_b, resp_a, resp_b))
        self.size = len(que_a) + len(que_b)
    
    def run(self, que_a, que_b, resp_a, resp_b): 
        
        for i in range(self.size):
            
            if(len(que_a) != 0 or len(que_b) != 0):
                
                if((len(que_a) == 0 and env.now < que_b[0]) or (len(que_b) == 0 and env.now < que_a[0]) or \
                        ((len(que_a) != 0 and len(que_b) != 0) and (env.now < que_a[0] and env.now < que_b[0]))):
                    
                    if (len(que_a) != 0 and len(que_b) != 0 ):
                        if(que_a[0] < que_b[0]):
                            yield self.env.timeout(que_a[0] - env.now)
                        else:
                            yield self.env.timeout(que_b[0] - env.now)
                    elif(len(que_a) == 0):
                        yield self.env.timeout(que_b[0] - env.now)
                    elif(len(que_b) == 0):
                        yield self.env.timeout(que_a[0] - env.now)
                        
                if((len(que_a) != 0) and (len(que_b) != 0)):
                    if(env.now >= que_a[0] and env.now >= que_b[0]):
                        if(que_a[0] < que_b[0]):
                            yield self.env.timeout(resp_a[0])
                            print("[Queue A] arr_t: {} ; resp {}".format(que_a[0],env.now))
                            que_a.pop(0)
                            resp_a.pop(0)
                        else:
                            yield self.env.timeout(resp_b[0])
                            print("[Queue B] arr_t: {} ; resp {}".format(que_b[0],env.now))
                            que_b.pop(0)
                            resp_b.pop(0)
                    
                    elif(env.now >= que_a[0]):
                        yield self.env.timeout(resp_a[0])
                        print("[Queue A] arr_t: {} ; resp {}".format(que_a[0],env.now))
                        que_a.pop(0)
                        resp_a.pop(0)

                    elif(env.now >= que_b[0]):
                        yield self.env.timeout(resp_b[0])
                        print("[Queue B] arr_t: {} ; resp {}".format(que_b[0],env.now))
                        que_b.pop(0)
                        resp_b.pop(0)

                        
                elif((len(que_a) != 0 and env.now >= que_a[0]) ):
                    yield self.env.timeout(resp_a[0])
                    print("[Queue A] arr_t: {} ; resp {}".format(que_a[0],env.now))
                    que_a.pop(0)
                    resp_a.pop(0)
                
                elif(len(que_b) != 0 and env.now >= que_b[0]):
                    yield self.env.timeout(resp_b[0])
                    print("[Queue B] arr_t: {} ; resp {}".format(que_b[0],env.now))
                    que_b.pop(0)
                    resp_b.pop(0)
                                
        print("len_q_a: {} len_q_b: {} q_a: {} q_b: {} resp_a: {} resp_b: {}".format(len(que_a),len(que_b), que_a, que_b, resp_a, resp_b))
    

if __name__ == "__main__":

    num_req = 100
    random_obj = random_gen()

    # arrival time : 5ms avg 
    # requests : 100
    queue = random_obj.random_poisson(5,num_req,False,True) 

    # make two queues for different requests 
    # size = 0.1*size_of_queue
    queue_a, queue_b = random_obj.random_select(queue, int(0.1*num_req))

    # response time for queue_a
    # lower bound : 3 ms
    # upper bound : 20 ms
    # size : len(queue_a)

    resp_list_a = random_obj.random_uniform(3,20,len(queue_a))


    # response time for queue_b
    # lower bound : 200 ms
    # upper bound : 1000 ms
    # size : len(queue_b)

    resp_list_b = random_obj.random_uniform(200,1000,len(queue_b))

    # provide initial time
    env = simpy.Environment(queue[0])

    serv = server(env, queue_a, queue_b, list(resp_list_a), list(resp_list_b))
    #print("queues : {} {} {}".format(queue, queue_a, queue_b))
    #print("resp : {} {} ".format(resp_list_a, resp_list_b))
    
    runtime = sum(resp_list_a) + sum(queue_a) + sum(queue_b) + sum(resp_list_b)
    print("runtime {}".format(runtime))
    env.run(runtime)

