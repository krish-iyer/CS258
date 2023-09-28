# this is the server requester

# import requests
# import time

# url = "http://localhost:8080/"
# max_req = 5000
# tik = time.time()
# print("Requests is being sent...")

# for i in range(max_req):
#     requests.get(url, timeout=0.001)

# print(f"This program had been running for {round(time.time()-tik,2)} seconds")

# The below code is adapted from https://stackoverflow.com/questions/16399355/refresh-a-local-web-page-using-python
# and mofdified to the need of solving the HW
from selenium import webdriver
import time
import urllib

max_req = 5000
url = "http://localhost:8080/" 
refreshrate = 0.001
driver = webdriver.Firefox()
driver.get(url)
driver.refresh()
tik = time.time()

for i in range(max_req):
    driver.refresh()
print(f"This program had been running for {round(time.time()-tik,2)} seconds")
