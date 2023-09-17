from random_gen import random_gen

class server():
    def __init__(self, env, que, resp_list):
        self.env = env
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
        self.action = env.process(self.run(que, resp_list))

        
    def run(self, que, resp_list): 
        
        while (len(que)) != 0:
            
            if(self.env.now < que[0]):    
                yield self.env.timeout(que[0] - self.env.now)
        
            if(self.env.now >= que[0]):
                yield self.env.timeout(resp_list[0])
                print("[Queue] arr_t: {} ; resp {} ; now{}".format(que[0],resp_list[0],self.env.now))
                self.server_resp.append(self.env.now - que[0])
                que.pop(0)
                resp_list.pop(0)

        
        self.end_time = self.env.now
        server_resp_list = (list(self.server_resp_list))
        self.misc_avg_resp = random_gen.mean(server_resp_list)
        self.arrival_rate = (self.size)/(self.end_time-self.start_time)
        self.server_util = self.arrival_rate*(self.misc_avg_resp)
        self.server_std_resp = random_gen.std(self.server_resp)       
        self.server_avg_resp = random_gen.mean(self.server_resp)