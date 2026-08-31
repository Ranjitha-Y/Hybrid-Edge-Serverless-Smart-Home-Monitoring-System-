# Hybrid Edge–Serverless IoT Motion Detection System

## Overview

This project implements a Hybrid Edge–Serverless architecture for low-latency IoT motion detection using an ESP32, PIR motion sensor, and AWS cloud services.

The system processes time-critical motion detection locally at the edge for rapid response while simultaneously communicating with cloud services for scalable processing, data storage, and notification workflows.

## Architecture

**ESP32 + PIR Sensor → Edge Decision → LED Response**

**ESP32 → API Gateway → AWS Lambda → DynamoDB / Amazon SNS**

## Technologies Used

* ESP32
* PIR Motion Sensor
* Arduino IDE
* C/C++
* AWS API Gateway
* AWS Lambda
* Amazon DynamoDB
* Amazon SNS
* REST API
* Edge Computing
* Serverless Computing

## Features

* Real-time motion detection using PIR sensor
* Local edge-based decision making
* Immediate LED response at the edge
* Cloud-based event processing using AWS Lambda
* REST communication through Amazon API Gateway
* Motion event storage using DynamoDB
* Notification workflow using Amazon SNS
* Hybrid and Cloud-Only performance comparison

## Hybrid Architecture

In Hybrid mode, the ESP32 makes the immediate motion-response decision locally. This reduces the latency of time-critical actions while cloud services handle additional processing and storage.

## Cloud-Only Architecture

In Cloud-Only mode, motion events are sent to the AWS backend before the response decision is processed, allowing comparison of cloud round-trip latency with local edge processing.

## Performance Evaluation

The system was evaluated by comparing edge response time and cloud round-trip time under Hybrid and Cloud-Only configurations.

The experimental results demonstrate the advantage of local edge processing for latency-sensitive IoT responses while retaining the scalability and flexibility of cloud-based services.


## Future Enhancements

* Add multiple ESP32 devices
* Implement MQTT communication
* Add a real-time monitoring dashboard
* Improve notification reliability
* Extend the system to additional IoT sensors
