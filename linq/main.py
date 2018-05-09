import numpy as np


class Container:
    def __init__(self, lst=None, producer=None):
        self.container = []
        self.len = None
        self.ind = -1
        if producer:
            self.producer = producer
        else:
            self.container = lst
            self.len = len(lst)

    def has_next(self):
        if self.container:
            return self.ind + 1 < self.len
        else:
            return self.producer.has_next()

    def get_next(self):
        if self.container:
            self.ind += 1
            return self.container[self.ind]
        else:
            return self.producer.get_next()

    def get_len(self):
        return self.len


class Sequence:

    def __init__(self, lst=None, producer=None):
        self.container = Container(lst=lst, producer=producer)
        self.sort = None
        self.group = None
        self.maps = []
        self.computed = False

    def compute(self):
        tmp_len = np.zeros(len(self.maps))
        limit_reached = False
        flat_stack = [(self.container, 0)]
        result = []
        while not limit_reached:
            filtered = False

            while len(flat_stack) > 0 and not flat_stack[-1][0].has_next():
                flat_stack.pop()
            if len(flat_stack) == 0:
                break

            tmp_op = flat_stack[-1][1]
            tmp = flat_stack[-1][0].get_next()

            for i in range(tmp_op, len(self.maps)):
                if self.maps[i][0] == 'select':
                    tmp = self.maps[i][1](tmp)
                elif self.maps[i][0] == 'filter':
                    if not self.maps[i][1](tmp):
                        filtered = True
                        break
                elif self.maps[i][0] == 'limit':
                    tmp_len[i] += 1
                    if tmp_len[i] == self.maps[i][1]:
                        limit_reached = True
                elif self.maps[i] == 'flatten':
                    flat_stack.append((Container(lst=tmp.to_list()), i + 1))
                    filtered = True
                    break
                else:
                    print(self.maps[i][0])

            if not filtered:
                result.append(tmp)

        if self.group:
            grouped = dict()
            for el in result:
                key = self.group(el)
                if not key in grouped:
                    grouped[key] = list()
                grouped[key].append(el)
            result = [(key, grouped[key]) for key in grouped]

        if self.sort:
            result = sorted(result, key=self.sort)

        self.computed = True
        self.container = Container(result)

    def select(self, foo):
        self.maps.append(('select', foo))
        self.computed = False
        return self

    def flatten(self):
        self.maps.append('flatten')
        self.computed = False
        return self

    def where(self, foo):
        self.maps.append(('filter', foo))
        self.computed = False
        return self

    def take(self, k):
        self.maps.append(('limit', int(k)))
        return self

    def group_by(self, foo):
        self.group = foo
        self.computed = False
        seq = Sequence([self])
        return seq.flatten()

    def order_by(self, foo):
        self.sort = foo
        self.computed = False
        seq = Sequence([self])
        return seq.flatten()

    def to_list(self):
        if not self.computed:
            self.compute()
        return self.container.container


print(Sequence(lst=range(10)).where(lambda x : x % 2 == 0)
      .select(lambda x : x ** 2)
      .take(2)
      .to_list())


class Fibb:
    def __init__(self):
        self.first = 0
        self.second = 1

    def get_next(self):
        result = self.first + self.second
        self.first = self.second
        self.second = result
        print(result)
        return result

    def has_next(self):
        return True


print(Sequence(producer=Fibb())
      .where(lambda x : x % 3 == 0)
      .select(lambda x : x**2 if x % 2 == 0 else x)
      .take(5)
      .to_list())


class WordFile:

    def __init__(self):
        self.file = open("test.txt")
        self.lst = []

    def get_next(self):
        if len(self.lst) > 0:
            ln = self.lst[0]
            self.lst.pop(0)
            return ln
        else:
            ln = self.file.readline()
            if not ln == '':
                return ln
        return ''

    def has_next(self):
        if len(self.lst) > 0:
            return True
        else:
            ln = self.file.readline()
            if not ln == '':
                self.lst.append(ln)
                return True
        return False


print(Sequence(producer=WordFile())
      .select(lambda x: Sequence(lst=x.split(' ')))
      .flatten().group_by(lambda x: x.lower())
      .select(lambda x: (x[0], len(x[1]))).order_by(lambda x: -x[1])
      .take(10)
      .to_list())

