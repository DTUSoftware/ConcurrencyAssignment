FROM gcc:latest
COPY . /ConcurrencyAssignment
WORKDIR /ConcurrencyAssignment/
RUN make
CMD ["./bank"]
