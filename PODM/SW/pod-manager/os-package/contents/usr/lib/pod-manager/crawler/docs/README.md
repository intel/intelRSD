pod-manager-crawler
===================

script utility to explore resources, save its state and present in isolation.

# Operations

## *crawl*

**description:** Traverses graph of resources and saves it in file.

**usage:** `./crawler.py crawl <uri> <output-file-name>`

### crawl - examples

* Crawl services in local machine and save them in given file.

```
./crawler.py crawl resources.json
```

* Crawl resources served by remote machine and save them in given file.

```
./crawler.py crawl --remote=podm@10.91.48.200 resources.json
```

## *mock*

**description:** Runs a mock server at given port and present data retrieved by 'crawl' command

**usage:** `./crawler.py mock <resources-file-name> <starting-port>`

### mock - examples

* Present services stored in resources.json

```
./crawler.py mock resources.json 8000
```
