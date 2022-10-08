FROM ubuntu

RUN apt update && apt install g++-11 make -y

RUN mv /bin/g++-11 /bin/g++

CMD ["bash"]