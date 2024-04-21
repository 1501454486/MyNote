#该问题的主要难点：
#1.各种数据类型都有，需要提前归一化
#2.有多种数据类型，需要提前拉成一个列矩阵，在最后再拉回去
#3.多分类问题，需要设置多个分类器，最后根据theta的结果判断到底输出哪个
#4.这里的布尔值涉及one-hot编码处理
import csv
import numpy as np
import pandas as pd
import copy
import math

#-----------------------------------------------神经网络本体----------------------------------------------------------------------

#Relu激活函数，用于隐藏层
def Relu(inputs):
    return np.maximum(0,inputs)

#softmax激活函数，用于最后一层输出
def softmax(inputs):
    max_values = np.max(inputs,axis=1 ,keepdims= True)          #keepdims：保持维度
    slided_inputs = inputs - max_values             #防止指数爆炸
    exp_values = np.exp(slided_inputs)              #分母
    norm_base = np.sum(exp_values, axis=1, keepdims=True)       #横着求和：样本是一行
    norm_value = exp_values / norm_base
    return norm_value

#标准化函数，用于层间：给每一层输出加上标准化，保证每一层输出总是处在[0,1]之间
def normalize(array):
    max_number = np.max(np.absolute(array),axis=1,keepdims=True)        #考虑到可能有负数，要用绝对值求每组的最大值
    scale_rate = np.where(max_number == 0,1,1/max_number)               #缩放倍率，保证最后最大的数乘以这个倍率是±1，同时防止被除数是0
    norm = array * scale_rate
    return norm

#标准化函数，用于数据
def data_normalize(array):
    max_number = np.max(np.absolute(array),axis=0,keepdims=True)
    scale_rate = np.where(max_number == 0,1,1/max_number)
    norm = array * scale_rate
    return norm

#向量标准化函数，因为向量不能用上面的标准化函数
def vector_normalize(array):
    max_number = np.max(np.absolute(array))                             #考虑到可能有负数，要用绝对值求每组的最大值
    scale_rate = np.where(max_number == 0,0,1/max_number)               #缩放倍率，保证最后最大的数乘以这个倍率是±1
    norm = array * scale_rate
    return norm


#分类函数：传入一个nX3的概率矩阵，传出ABC字符列表
def classify(probabilities):
    classify_result = []
    for i in range(len(probabilities)):
        if probabilities[[i],[0]] > (1/3):
            classify_result.append('A')
        elif probabilities[[i],[1]] > (1/3):
            classify_result.append('B')
        else:
            classify_result.append('C')
    return np.array(classify_result)

#需求函数，即最后输出的时候需要怎么变，用-1表示需要减小，0表示不变，1表示需要增大
def get_final_layer_preAct_demands(predicted_values,target_matrix):     #输入实际预测的值和目标值（真实值），target
    for i in range(len(target_matrix)):
        if np.dot(target_matrix[i,:],predicted_values[i]) > (1/3):
            target_matrix[i,:] = np.array([0,0,0])
        else:
            target_matrix[i,:] = (target_matrix[i,:] - 0.5) * 2
    return target_matrix                                                #返回需求矩阵

#损失函数
def precise_lossfunction(predicted,real_matrix):            #传入预测概率矩阵(NX3)和真实值矩阵(NX3)                                 #总样本数等于矩阵的行数
    product = np.sum(real_matrix*predicted,axis=1)        #花乘，保持矩阵形状不变，交叉熵函数
    return (1 - product)

def lossfunction(predicted,real_matrix):            #传入预测概率矩阵(NX3)和真实值矩阵(NX3)
    condition = (predicted > (1/3))
    binary_predicted = np.where(condition,1,0)

    product = np.sum(real_matrix*binary_predicted,axis=1)        #花乘，保持矩阵形状不变，交叉熵函数
    return (1 - product)


#充分利用py面向对象的特点，将神经网络的生成封装成class！女少口阿
#定义一个层
class Layer:
    def __init__(self,n_inputs,n_neurons):
        self.weights = np.random.randn(n_inputs,n_neurons)
        self.biases = np.random.randn(n_neurons)

    def layer_forward(self,inputs):
        self.sum1 = np.dot(inputs, self.weights) + self.biases
        return self.sum1

    def layer_backward(self,preweights_values,afterweights_demands):
        preweights_demands = np.dot(afterweights_demands,self.weights.T)                 #注意反向传播的时候不能直接乘，要先转置

        condition = (preweights_values > 0)
        value_derivatives = np.where(condition,1,0)                                     #看调整前是否大于0，如果是导数就是1，否则是0
        preActs_demands = value_derivatives * preweights_demands
        norm_preActs_demands = normalize(preActs_demands)                               #如果不标准化，传播过程中可能发散

        weight_adjust_matrix = self.get_weight_adjust_matrix(preweights_values,afterweights_demands)
        norm_weight_adjust_matrix = normalize(weight_adjust_matrix)

        return (norm_preActs_demands,norm_weight_adjust_matrix)

    def get_weight_adjust_matrix(self,preweight_values,afterWeights_demands):           #调整矩阵
        plain_weights = np.full(self.weights.shape,1.0)
        weights_adjust_matrix = np.full(self.weights.shape,0.0)
        plain_weights_T = plain_weights.T

        for i in range(BATCH_SIZE):
            weights_adjust_matrix += (plain_weights_T*preweight_values[i,:]).T*afterWeights_demands[i,:]
        weights_adjust_matrix = weights_adjust_matrix / BATCH_SIZE
        return weights_adjust_matrix

#定义一个网络类
class Network:
    def __init__(self,network_shape):
        self.shape = network_shape                                      #网络的形状
        self.layers = []
        for i in range(len(network_shape)-1):                           #-1是因为定义四层，第零层是输入层
            layer = Layer(network_shape[i],network_shape[i+1])
            self.layers.append(layer)
    #定义前馈运算
    def network_forward(self,inputs):
        outputs = [inputs]
        for i in range(len(self.layers)):
            layer_sum = self.layers[i].layer_forward(outputs[i])
            if i < len(self.layers)-1:
                layer_output= Relu(layer_sum)                           #将激活函数放在网络中而不是层中，方便不同层使用不同的激活函数
            else:
                layer_output = softmax(layer_sum)
            outputs.append(layer_output)
        return outputs
    #网络的反向传播
    def network_backward(self,layer_outputs,target_vector):             #目标向量就是标签
        backup_network = copy.deepcopy(self)                            #备用网络，因为不确定更新网络后是否比原来的网络要好
        preAct_demands = get_final_layer_preAct_demands(layer_outputs[-1],target_vector)
        for i in range(len(self.layers)):                               #网络有几层就循环几次
            layer = backup_network.layers[len(self.layers) - (1+i)]     #注意减去的是i+1，因为网络是从第0层开始的
            if i != 0:                                                  #注意最后一层不要调整biases，否则容易产生过拟合！！
                layer.biases += LEARNING_RATE * np.mean(preAct_demands,axis=0)          #axis=0是因为对同一特征，在不同批次中求平均值！同时还需要乘学习率，方便调整
                layer.biases = vector_normalize(layer.biases)
            #更新权重矩阵
            outputs = layer_outputs[len(layer_outputs) - (2+i)]           #同的是上上层的输出值
            result_list = layer.layer_backward(outputs, preAct_demands)
            preAct_demands = result_list[0]
            weight_adjust_matrix = result_list[1]
            layer.weights += LEARNING_RATE * weight_adjust_matrix
            layer.weights = normalize(layer.weights)

        return backup_network                                           #返回更新后的备用网络，后面与原来的网络比较后选择

    #单批次训练函数
    def one_batch_train(self,batch):                                  #batch的最后三列是标签值
        inputs = batch[:,:-3]                                         #特征值
        targets = copy.deepcopy(batch[:,-3:])                           #目标值
        outputs = self.network_forward(inputs)
        precise_loss = precise_lossfunction(outputs[-1],targets)
        loss = lossfunction(outputs[-1],targets)

        if np.mean(precise_loss) <= 0.1:
            print('No need for training')
        else:
            backup_network = self.network_backward(outputs,targets)
            backup_outputs = backup_network.network_forward(inputs)
            backup_precise_loss = precise_lossfunction(backup_outputs[-1],targets)
            backup_loss = lossfunction(backup_outputs[-1],targets)

            if np.mean(precise_loss) >= np.mean(backup_precise_loss) or np.mean(loss) >= np.mean(backup_loss):                 #如果原来的网络损失值比新网络损失值大，表示这轮训练成功，更新网络
                print('Imporoved')
                for i in range(len(self.layers)):
                    self.layers[i].weights = backup_network.layers[i].weights.copy()            #就将新网络的数据拷贝过去，注意要用copy深度拷贝，否则两个内存地址相同，是浅拷贝，会出问题
                    self.layers[i].biases = backup_network.layers[i].biases.copy()
            else:
                print('No improvement')

    # 多批次训练，输入训练数据
    def train(self,batch):
        for i in range(len(batch)):
            self.one_batch_train(batch)


    # 测试，并将数据保存到同目录下的q5result.csv中
    def predict(self):
        X_test,X_raw = read_test_data(test_path)
        inputs = X_test
        outputs = self.network_forward(inputs)  # 训练完毕，向前传播计算
        prediction = classify(outputs[-1])  # 对最后一层的输出分类，这时候得到的prediction是字符串矩阵
        #输出矩阵：序号、X_test和choice拼接，然后再写入
        number = []
        for i in range(len(X_test)):
            number.append(i+1)
        number_matrix = np.array(number)
        matrix1 = np.append(number_matrix,X_raw)
        result_matrix = np.append(matrix1,prediction)                #这个result_matrix保存了所有的输出信息，接下来只需要写入

        #写入q5result.csv文件
        # with open('q5result.csv','w',encoding='UTF8',newline='') as f:        #注意加newline=''，否则写一行空一行
        #     writer = csv.writer(f)
        #     header = ['Stu ID','pos_x','pos_y','consume_level','have_class_afernoon','choice']
        #     writer.writerow(header)
        #     writer.writerows(result_matrix)
        np.savetxt('q5result.csv',result_matrix,fmt='%s')


#---------------------------------------------------我是分隔符-----------------------------------------------------------------------

NETWORK_SHAPE = [4,3]       #神经网络的形状：分别有几层，每层分别有几个神经元
#因为这里有四个输入特征，所以第零层创建四个神经元；由于要分类ABC，所以最后一层有三个输出神经元
BATCH_SIZE = 4900                  #训练多少次
LEARNING_RATE = 0.0005               #学习率，可以调整
iters = 1                       #迭代次数

#任务：读入数据到X和y中，并且把y进行one-hot编码处理
#创建一个神经网络
ACEE_network = Network(NETWORK_SHAPE)

#one-hot编码
def one_hot_encoder(y):
    result = []
    for i in y:
        y_temp = np.zeros(3)
        if i == 'A':
            y_temp[0] = 1
        elif i == 'B':
            y_temp[1] = 1
        else:
            y_temp[2] = 1
        result.append(y_temp)

    return np.array(result)

#写个函数用来读取数据，先用来读取训练数据，后用来读取测试数据，同时对y进行one-hot编码处理，返回两个矩阵
#开始本来想重复用的，但是后来发现测试的没有标签，这样会有问题，所以就没有重复使用
def read_train_data(path):
    data = pd.read_csv(path,header=0)
    X = np.array(data.iloc[:,1:-1]).astype(float)
    y_raw = np.array(data.iloc[:,-1])
    y = one_hot_encoder(y_raw)
    return X,y,y_raw

def read_test_data(path):
    data = pd.read_csv(path, header=0)
    X_raw = data.iloc[:, 1:]
    X_test = np.array(X_raw).astype(float)
    return X_test, X_raw


#读入训练数据，同时将X_train和y_train变成矩阵形式
train_path = 'q5train.csv'
test_path = 'q5test.csv'
X_train,y_train,y_raw = read_train_data(train_path)

X_train_norm = data_normalize(X_train)
print(X_train_norm)
train_batch = np.append(X_train_norm,y_train,axis=1)                     #沿着x轴将两个矩阵拼起来方便输入


#------------------------------炼丹炉开启！！！-----------------------------
ACEE_network.train(train_batch)