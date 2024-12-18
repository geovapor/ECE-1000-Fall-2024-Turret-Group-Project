import cv2
import numpy as np
import RPi.GPIO as GPIO
import time

# Relay pin setup
RELAY_PIN = 18  # Update this to your actual relay control pin
GPIO.setmode(GPIO.BCM)
GPIO.setup(RELAY_PIN, GPIO.OUT)
GPIO.output(RELAY_PIN, GPIO.HIGH)  # Turn the relay OFF initially (active-low)

# Define the HSV color ranges for detecting dark blue and green
lower_dark_blue = np.array([75, 115, 0])
upper_dark_blue = np.array([130, 255, 255])

lower_green = np.array([50, 15, 150])
upper_green = np.array([90, 255, 255])

# Start the webcam capture
cap = cv2.VideoCapture(0)

# Resize scale for faster processing
resize_scale = 0.5  # Scale down for faster processing

# Debounce logic for relay
last_relay_state = GPIO.HIGH
last_switch_time = time.time()
debounce_delay = 0.5  # in seconds

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Failed to capture frame. Retrying...")
            continue

        # Resize the frame
        frame = cv2.resize(frame, (0, 0), fx=resize_scale, fy=resize_scale)

        # Convert the frame from BGR to HSV
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        # Create masks for detecting dark blue and green colors
        mask_dark_blue = cv2.inRange(hsv, lower_dark_blue, upper_dark_blue)
        mask_green = cv2.inRange(hsv, lower_green, upper_green)

        # Display masks for debugging (optional)
        # cv2.imshow("Dark Blue Mask", mask_dark_blue)
        # cv2.imshow("Green Mask", mask_green)

        # Detect contours of the green laser
        green_contours, _ = cv2.findContours(mask_green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        laser_detected = False

        for contour in green_contours:
            if cv2.contourArea(contour) > 100:  # Adjust threshold as needed
                laser_detected = True
                x, y, w, h = cv2.boundingRect(contour)
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        # Check if the green laser is hitting the dark blue surface
        blue_hits = cv2.bitwise_and(mask_dark_blue, mask_green)
        if np.sum(blue_hits) > 0:
            cv2.putText(frame, "Laser on Dark Blue Surface!", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
            if time.time() - last_switch_time > debounce_delay:
                GPIO.output(RELAY_PIN, GPIO.LOW)  # Turn relay ON
                last_relay_state = GPIO.LOW
                last_switch_time = time.time()
                print("Relay ON: Laser detected on dark blue surface.")
        else:
            if time.time() - last_switch_time > debounce_delay and last_relay_state != GPIO.HIGH:
                GPIO.output(RELAY_PIN, GPIO.HIGH)  # Turn relay OFF
                last_relay_state = GPIO.HIGH
                last_switch_time = time.time()
                print("Relay OFF: No laser on dark blue surface.")

        # Display the original frame
        cv2.imshow("Laser and Dark Blue Surface Detection", frame)

        # Wait for key press: 'q' to quit
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            break

except KeyboardInterrupt:
    print("Interrupted by user.")
finally:
    # Clean up
    cap.release()
    cv2.destroyAllWindows()
    GPIO.cleanup()