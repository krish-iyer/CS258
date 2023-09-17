import numpy as np

class random_gen():
    
    def __init__(self):
        self.rn_gen = np.random.default_rng()
    
    '''
        To generate random number with mean and standard deviation
        @params:
            mean : mean of the list to be returned
            std  : standard deviation allowed between the numbers
            size : size of the list of number to generated
            round : round all values to 0 decimal place
            cumulate : Cumulate all the value starting from index 0
    '''
    def random_mean(self, mean, std, size, round=False, cumulate=False):
        random_number = []

        random_number = (self.rn_gen.normal(mean, std, size))
        
        if round == True:
            random_number = np.round(random_number)
            
        if cumulate == True:
            random_number = np.cumsum(random_number)
            
        return random_number

    '''
        To generate random number with poisson distribution with mean and size
        @params:
            mean : mean of the list to be returned
            size : size of the list of number to generated
            round : round all values to 0 decimal place
            cumulate : Cumulate all the value starting from index 0
    '''
    def random_poisson(self, mean, size, round=False, cumulate=False):
        random_number = []

        random_number = (self.rn_gen.poisson(mean,size))

        if round == True:
            random_number = np.round(random_number)
            
        if cumulate == True:
            random_number = np.cumsum(random_number)
        return random_number

    '''
        To generate random number with poisson distribution with mean and size
        @params:
            lower_b : lower bound value
            upper_b : upper bound value
            size    : size of the list of number to generated
            round : round all values to 0 decimal place
            cumulate : Cumulate all the value starting from index 0
    '''
    def random_uniform(self, lower_b, upper_b, size, round=False, cumulate=False):
        random_number = []

        random_number = (self.rn_gen.uniform(lower_b, upper_b, size))

        if round == True:
            random_number = np.round(random_number)

        if cumulate == True:
            random_number = np.cumsum(random_number)
            
        return random_number

    '''
        To select random elements from the list
        @params:
            que : list of numbers
            size    : size of the list of number to generated
            replace : repeat for selecting the numbers
            
        @return:
            unique_que : difference of the que and selected numbers
            rand_c : randomly chosen numbers
    '''
    def random_select(self, que, size, replace=False):
            
        unique_que = list(que)
        rand_c = np.random.choice(que, size, replace=replace)
        for num in rand_c:
            unique_que.remove(num)
        #print("random_selection {} {}".format(que,rand_c))
        return sorted(unique_que), sorted(rand_c)
    
    def std(que):
        return np.std(que)
    
    def mean(que):
        return np.mean(que)
     
if __name__ == "__main__":

    import matplotlib.pyplot as plot
    random_obj = random_gen()
    random_ = random_obj.random_mean(5,10, 1000)
    #print(np.round(random_))
    plot.plot((random_))
    plot.show()