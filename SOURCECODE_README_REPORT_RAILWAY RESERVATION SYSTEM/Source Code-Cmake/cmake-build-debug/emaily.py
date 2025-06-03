import sys
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

def send_email(to_email, filename):
    from_email = "policedtb.csproject@gmail.com"
    password = "iwgvqdrpqinmlfpj"
    # Read the email content from the file
    with open(filename, 'r') as file:
        email_content = file.read()

    # Construct the email message
    msg = MIMEMultipart()
    msg['From'] = from_email
    msg['To'] = to_email
    msg['Subject'] = "Travel Plan Details"  # Set the subject here
    msg.attach(MIMEText(email_content, 'plain'))

    # Connect to SMTP server and send the email
    try:
        server = smtplib.SMTP('smtp.gmail.com', 587)
        server.starttls()
        server.login(from_email, password)
        server.sendmail(from_email, to_email, msg.as_string())
        print("Email sent successfully.")
    except Exception as e:
        print(f"Failed to send email: {e}")
    finally:
        server.quit()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: send_email.py <to_email> <filename>")
        sys.exit(1)

    to_email = sys.argv[1]
    filename = sys.argv[2]
    send_email(to_email, filename)
