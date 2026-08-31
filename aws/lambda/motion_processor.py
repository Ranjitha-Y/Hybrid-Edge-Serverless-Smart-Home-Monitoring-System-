import json
import uuid
from datetime import datetime, timezone

import boto3

dynamodb = boto3.resource("dynamodb")
table = dynamodb.Table("MotionLogs")

sns = boto3.client("sns")

SNS_TOPIC_ARN = "arn:aws:sns:us-east-1:476114124842:MotionAlertsTest"


def lambda_handler(event, context):
    print("Received event:", json.dumps(event))

    # Get body from API Gateway or direct Lambda test
    body = event.get("body", event)

    if isinstance(body, str):
        body = json.loads(body)

    device_id = body.get("deviceId", "ESP32-01")
    event_type = body.get("eventType", "MOTION_DETECTED")
    mode = body.get("mode", "HYBRID")

    event_id = str(uuid.uuid4())
    timestamp = datetime.now(timezone.utc).isoformat()

    # Store event in DynamoDB
    table.put_item(
        Item={
            "eventId": event_id,
            "deviceId": device_id,
            "eventType": event_type,
            "mode": mode,
            "timestamp": timestamp
        }
    )

    # Send SNS email notification
    message = (
        f"Motion detected!\n\n"
        f"Device: {device_id}\n"
        f"Event: {event_type}\n"
        f"Mode: {mode}\n"
        f"Time (UTC): {timestamp}"
    )

    sns.publish(
        TopicArn=SNS_TOPIC_ARN,
        Subject="Smart Home Motion Alert",
        Message=message
    )

    # Command returned to ESP32
    led_command = "ON" if mode == "CLOUD_ONLY" else "NONE"

    return {
        "statusCode": 200,
        "body": json.dumps({
            "message": "Motion processed successfully",
            "eventId": event_id,
            "mode": mode,
            "ledCommand": led_command
        })
    }