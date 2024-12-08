from os import getenv
from dotenv import load_dotenv
load_dotenv()
USER_TOKEN = getenv("USER_TOKEN")
import asyncio

from PyCharacterAI import get_client
from PyCharacterAI.exceptions import SessionClosedError

character_id = "VIqELxEyIJttOJVzR_sYEDCOBq9OHuYBTmaEX97lFLw" # Astolfo cid


async def main():
    client = await get_client(token=USER_TOKEN)

    me = await client.account.fetch_me()
    print(f"Authenticated as @{me.username}")

    chat, greeting_message = await client.chat.create_chat(character_id)

    print(f"{greeting_message.author_name}: {greeting_message.get_primary_candidate().text}")

    try:
        while True:
            # NOTE: input() is blocking function!
            message = input(f"[{me.name}]: ")

            answer = await client.chat.send_message(character_id, chat.chat_id, message)
            print(f"[{answer.author_name}]: {answer.get_primary_candidate().text}")
    except SessionClosedError:
        print("session closed. Bye!")

    finally:
        # Don't forget to explicitly close the session
        await client.close_session()

asyncio.run(main())