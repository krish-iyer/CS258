from random_gen import random_gen

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
        self.server_resp_a = []
        self.server_resp_b = []
        self.server_resp = []
        self.misc_avg_resp = None
        self.misc_avg_resp_a = None
        self.misc_avg_resp_b = None
        self.que = que.copy()
        self.resp_list = resp_list.copy()
        self.start_time_long = None
        self.end_time_long = None
        self.arrival_rate_long = None
        self.server_util_long = None
        
        for i in range(len(que)):
            if(resp_list[i] >= 200):
                self.start_time_long = resp_list[i]
                break
            
        for i in range(self.proc):
            self.clock = [que[i] for i in range(self.proc)]
            
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
        
        while (len(self.que)) != 0:
            
            proc = self.nearest()
            if(self.clock[proc] < self.que[0]):    
                self.wait(proc, self.que[0] - self.clock[proc])
        
            if(self.clock[proc] >= self.que[0]):
                self.wait(proc, self.resp_list[0])
                #print("[Queue] arr_t: {} ; resp {} ; now{}".format(self.que[0],self.resp_list[0],self.clock[proc]))
                if(self.resp_list[0] >= 200):
                    self.server_resp_b.append(self.clock[proc] - self.que[0])
                    self.end_time_long = self.que[0]
                else:
                    self.server_resp_a.append(self.clock[proc] - self.que[0])
                    self.end_time = self.que[0]
                    
                self.server_resp.append(self.clock[proc] - self.que[0])
                self.que.pop(0)
                self.resp_list.pop(0)

        
        server_resp_list = (list(self.server_resp_list))
        self.misc_avg_resp = random_gen.mean(server_resp_list)
        self.server_avg_resp_a = random_gen.mean(self.server_resp_a)
        self.server_avg_resp_b = random_gen.mean(self.server_resp_b)
        
        self.arrival_rate = (self.size)/(self.end_time-self.start_time)
        self.server_util = self.arrival_rate*(self.server_avg_resp_a)
        self.arrival_rate_long = (0.1*self.size)/(self.end_time_long - self.start_time_long)
        self.server_util_long = self.arrival_rate_long*(self.server_avg_resp_b)
        
        self.server_std_resp = random_gen.std(self.server_resp)       
        self.server_avg_resp = random_gen.mean(self.server_resp)
        
