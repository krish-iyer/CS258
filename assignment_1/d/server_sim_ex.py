from random_gen import random_gen
import numpy as np

class server():
    def __init__(self, que, resp_list, proc):
    
        self.proc = proc
        self.size = len(que)
        self.server_resp_list = resp_list.copy()
        self.server_avg_resp = None
        self.server_std_resp = None
        self.end_time = None
        self.start_time = que[0]
        self.arrival_rate = None
        self.server_util = None
        self.server_resp = []
        self.misc_avg_resp = None
        self.que = que.copy()
        self.resp_list = resp_list.copy()

        
        for i in range(self.proc):
            self.clock = [que[i] for i in range(self.proc)]
            
        self.multi_queue, self.multi_resp = self.create_multi_queue()
        # print("multi queue: {}".format(self.multi_queue))
        # print("multi_resp : {}".format(self.multi_resp))
        
    def create_multi_queue(self):
        multi_queue = []
        multi_resp = []
        
        #print(self.que)
        
        for i in range(self.proc):
            multi_queue.append([self.que[i]])
            multi_resp.append([self.resp_list[i]])
                
        for i in range(self.proc):
            self.que.pop(0)
            self.resp_list.pop(0)
            
        
        
        for i in range(len(self.que)):
            mean_of_lists = [np.mean(ls) for ls in multi_queue]
            min_of_mean = min(mean_of_lists)
            indx = mean_of_lists.index(min_of_mean)
            multi_queue[indx].append(self.que[i])
            multi_resp[indx].append(self.resp_list[i])
            

        #print([np.mean(ls) for ls in multi_queue])
        
        return multi_queue, multi_resp
    def nearest(self):
        val = min(self.clock)
        index = self.clock.index(val)
        return index
        
    def serve(self):
        self.run()
        
    def wait(self, proc, time):
        #print("[INC] Proc : {} current: {} time: {}".format(proc, self.clock[proc], time))
        self.clock[proc] += time
        
    def run(self): 
        proc = 0
        for que, resp_list in zip(self.multi_queue,self.multi_resp):

            while (len(que)) != 0:
                
                if(self.clock[proc] < que[0]):    
                    self.wait(proc, que[0] - self.clock[proc])
            
                if(self.clock[proc] >= que[0]):
                    self.wait(proc, resp_list[0])
                    # print("[Queue] arr_t: {} ; resp {} ; now{}".format(que[0],resp_list[0],self.clock[proc]))
                    self.server_resp.append(self.clock[proc] -que[0])
                    que.pop(0)
                    resp_list.pop(0)
            proc += 1
        
        self.end_time = max(self.clock)
        server_resp_list = (list(self.server_resp_list))
        self.misc_avg_resp = random_gen.mean(server_resp_list)
        self.arrival_rate = (self.size)/(self.end_time-self.start_time)
        self.server_util = self.arrival_rate*(self.misc_avg_resp)
        self.server_std_resp = random_gen.std(self.server_resp)       
        self.server_avg_resp = random_gen.mean(self.server_resp)
