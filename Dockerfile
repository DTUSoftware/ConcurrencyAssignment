FROM gcc:latest
COPY . /ConcurrencyAssignment
WORKDIR /ConcurrencyAssignment/
RUN gcc -o bank main.c main.h
CMD ["./bank"]
