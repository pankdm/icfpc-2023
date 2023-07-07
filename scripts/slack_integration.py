import sys


def post_to_slack(s):
    try:
        import os
        import slack

        print("Sending to slack")
        # If lost, retrieve the token from here: https://api.slack.com/apps/AKT5B5SJG/oauth
        client = slack.WebClient(token=os.environ["SLACK_API_TOKEN"])
        response = client.chat_postMessage(channel="#bot-logs", text=s)
        return response
    except:
        print("[slack] Unexpected error:", sys.exc_info()[0:2])


if __name__ == "__main__":
    response = post_to_slack("Hello hackers!")
    print(response)
    assert response["ok"]
    assert response["message"]["text"] == "Hello hackers!"
