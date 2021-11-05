FROM python:3.9-slim

EXPOSE 4242

RUN pip install kbw

CMD python -m kbw