FROM ubuntu:10.04

# edit /etc/apt/sources.list for this old-ass version
RUN perl -i.bak -pe 's/archive/old-releases/' /etc/apt/sources.list
RUN apt-get update \
 && apt-get install -y noweb make g++ texlive

CMD ["bash"]
