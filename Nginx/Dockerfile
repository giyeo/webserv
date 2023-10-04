FROM debian:oldstable

RUN apt-get update

RUN apt-get install nginx openssl -y

COPY default /etc/nginx/sites-available/default

WORKDIR /var/www/cgi-bin

COPY hello.py .
EXPOSE 81

ENTRYPOINT ["nginx", "-g", "daemon off;"]