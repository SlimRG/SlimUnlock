import aiohttp
import asyncio
from bs4 import BeautifulSoup
import sys

async def send_request(url) -> str:
    async with aiohttp.ClientSession() as session:
        async with session.get(url) as resp:
            return await resp.text(encoding="utf-8")
            
async def parse_category(category_url):
    html_response = await send_request(category_url)
    soup = BeautifulSoup(html_response, "lxml")
    teashous = soup.find_all("p", class_="teashou")
    for teashou in teashous:      
        if ("BIO" not in teashou.text): 
            continue      
        url = teashou.find('a', href=True)['href'].replace("../", "") 
        url = 'http://www.huananzhi.com/' + url       
        print(url)
        
                
async def main(url):
    task = asyncio.create_task(parse_category(sys.argv[1]))
    await task
    

if __name__ == "__main__":      
    asyncio.run(parse_category(sys.argv[1]))