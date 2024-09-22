#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/long_array_generated.h"
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/str_array_generated.h"

#include<time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <sstream>
extern "C" {
typedef struct emo{
    char judge[5];
    char word[20];
    char sentence[200];
}emo;
std::vector<std::string> split_sentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::string sentence;
    for (char ch : text) {
        sentence += ch;
        if (ch == '.' || ch == '!' || ch == '?') {
            sentences.push_back(sentence);
            sentence.clear();
        }
    }
    if (!sentence.empty()) {
        sentences.push_back(sentence);
    }
    return sentences;
}

// 分词函数
std::vector<std::string> tokenize(const std::string& sentence) {
    std::vector<std::string> tokens;
    std::string word;
    for (char ch : sentence) {
        if (std::isspace(ch) || std::ispunct(ch)) {
            if (!word.empty()) {
                tokens.push_back(word);
                word.clear();
            }
        } else {
            word += ch;
        }
    }
    if (!word.empty()) {
        tokens.push_back(word);
    }
    return tokens;
}
void write_to_file(const char* file_path,const char* docker_to_host_signal, char* buffer, size_t size) {
    int fd = open(file_path, O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (write(fd, buffer, size) == -1) {
        perror("write");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if(close(fd)==-1)
    {
       perror("close");
       printf("close docker_to_host failed\n");
       exit(EXIT_FAILURE);
    }
    int fd1 = open(docker_to_host_signal, O_WRONLY | O_CREAT, 0666);
    if (fd1 == -1) {
        perror("open");
        //close(fd1);
        exit(EXIT_FAILURE);
    }
    if(close(fd1)==-1)
    {
        perror("close");
        printf("close docker_to_host signal failed\n");
        exit(EXIT_FAILURE);
    }
    //printf("write docker_to_host and signal successfully!\n");
}

void read_from_file(const char* file_path,const char* host_to_docker_signal, char* buffer, size_t size) {
    int fd,fd1,fd2;
    const char* end_path = "/mnt/ramdisk/end";
    while ((fd1 = open(host_to_docker_signal, O_RDONLY)) == -1){
        if((fd2 = open(end_path, O_RDONLY)) != -1)
        {
            if(close(fd2)==-1)
            {
                perror("close");
                printf("close end failed\n");
                exit(EXIT_FAILURE);
            }
            return;

        }
        usleep(1);  // Sleep for 1us
    }
    if(fd1!=-1)
    {
        if(close(fd1)==-1)
        {
            perror("close");
            printf("close host_to_docker_signal failed\n");
            exit(EXIT_FAILURE);
        }
    }
    else{
        printf("create docker_to_host signal failed\n");
    }
    if(unlink(host_to_docker_signal)==-1)
    {
        printf("delete host_to_docker_signal failed\n");
    }
    fd = open(file_path,O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (read(fd, buffer, size) == -1) {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if(close(fd)==-1)
    {
        perror("close");
        printf("close host_to_docker failed\n");
        exit(EXIT_FAILURE);
    }
    if(unlink(file_path)==-1)
    {
        printf("delete host_to_docker failed\n");
    } // 删除文件
    //printf("read host_to_docker and signal successfully!\n"); 
}
int main(){
 // listen socket
    const std::unordered_set<std::string> positive_words = {"adorable", "accepted", "achievement", "active", "admire", "adventure", "affirmative", "affluent", "agree", "agreeable", "amazing", "angelic", "appealing", "approve", "aptitude", "attractive", "awesome", "beaming", "beautiful", "believe", "beneficial", "bliss", "bountiful", "bounty", "brave", "bravo", "brilliant", "celebrated", "champ", "champion", "charming", "cheery", "classic", "classical", "clean", "commend", "composed", "congratulation", "constant", "cool", "courageous", "creative", "cute", "dazzling", "delight", "delightful", "distinguished", "divine", "earnest", "easy", "ecstatic", "effective", "effervescent", "efficient", "effortless", "electrifying", "elegant", "enchanting", "encouraging", "endorsed", "energetic", "energized", "engaging", "enthusiastic", "essential", "esteemed", "ethical", "excellent", "exciting", "exquisite", "fabulous", "fair", "familiar", "famous", "fantastic", "favorable", "fetching", "fine", "fitting", "flourishing", "fortunate", "free", "fresh", "friendly", "fun", "funny", "generous", "genius", "genuine", "giving", "glamorous", "glowing", "good", "gorgeous", "graceful", "great", "green", "grin", "growing", "handsome", "happy", "harmonious", "healing", "healthy", "hearty", "heavenly", "honest", "honorable", "honored", "ideal", "imaginative", "imagine", "impressive", "independent", "innovate", "innovative", "instant", "instantaneous", "instinctive", "intuitive", "intellectual", "intelligent", "inventive", "jovial", "joy", "jubilant", "keen", "kind", "knowing", "knowledgeable", "laugh", "legendary", "light", "learned", "lively", "lovely", "lucid", "lucky", "luminous", "marvelous", "masterful", "meaningful", "merit", "meritorious", "miraculous", "motivating", "moving", "natural", "nice", "novel", "nurturing", "nutritious", "okay", "open", "optimistic", "paradise", "perfect", "phenomenal", "pleasurable", "plentiful", "pleasant", "poised", "polished", "popular", "positive", "powerful", "prepared", "pretty", "principled", "productive", "progress", "prominent", "protected", "proud", "quality", "quick", "quiet", "ready", "reassuring", "refined", "refreshing", "rejoice", "reliable", "remarkable", "resounding", "respected", "restored", "reward", "rewarding", "right", "robust", "safe", "satisfactory", "secure", "seemly", "simple", "skilled", "skillful", "smile", "soulful", "sparkling", "special", "spirited", "spiritual", "stirring", "stupendous", "stunning", "success", "successful", "sunny", "super", "superb", "supporting", "surprising", "terrific", "thorough", "thrilling", "thriving", "tops", "tranquil", "transforming", "transformative", "trusting", "truthful", "unreal", "unwavering", "upbeat", "upright", "upstanding", "valued", "vibrant", "victorious", "victory", "vigorous", "virtuous", "vital", "vivacious", "wealthy", "welcome", "well", "whole", "wholesome", "willing", "wonderful", "wondrous", "worthy", "wow", "yummy", "zeal", "zealous"};
    const std::unordered_set<std::string> negative_words = {"abysmal", "adverse", "alarming", "angry", "annoy", "anxious", "apathy", "appalling", "atrocious", "awful", "bad", "banal", "barbed", "belligerent", "bemoan", "beneath", "boring", "broken", "callous", "clumsy", "coarse", "cold", "collapse", "confused", "contradictory", "contrary", "corrosive", "corrupt", "crazy", "creepy", "criminal", "cruel", "cry", "cutting", "decaying", "damage", "damaging", "dastardly", "deplorable", "depressed", "deprived", "deformed", "deny", "despicable", "detrimental", "dirty", "disease", "disgusting", "disheveled", "dishonest", "dishonorable", "dismal", "distress", "dreadful", "dreary", "enraged", "eroding", "evil", "fail", "faulty", "feeble", "fight", "filthy", "foul", "frighten", "frightful", "gawky", "ghastly", "grave", "greed", "grim", "grimace", "gross", "grotesque", "gruesome", "guilty", "haggard", "hard", "hard-hearted", "harmful", "hate", "hideous", "homely", "horrendous", "horrible", "hostile", "hurt", "hurtful", "icky", "ignore", "ignorant", "ill", "immature", "imperfect", "impossible", "inane", "inelegant", "infernal", "injure", "injurious", "insane", "insidious", "insipid", "jealous", "junky", "lose", "lousy", "lumpy", "malicious", "mean", "menacing", "messy", "misshapen", "missing", "misunderstood", "moan", "moldy", "monstrous", "naive", "nasty", "naughty", "negate", "negative", "never", "nobody", "nondescript", "nonsense", "noxious", "objectionable", "odious", "offensive", "old", "oppressive", "pain", "perturb", "pessimistic", "petty", "plain", "poisonous", "poor", "prejudice", "questionable", "quirky", "quit", "reject", "renege", "repellant", "reptilian", "repulsive", "repugnant", "revenge", "revolting", "rocky", "rotten", "rude", "ruthless", "sad", "savage", "scare", "scary", "scream", "severe", "shoddy", "shocking", "sick", "sickening", "sinister", "slimy", "smelly", "sobbing", "sorry", "spiteful", "sticky", "stinky", "stormy", "stressful", "stuck", "stupid", "substandard", "suspect", "suspicious", "tense", "terrible", "terrifying", "threatening", "ugly", "undermine", "unfair", "unfavorable", "unhappy", "unhealthy", "unjust", "unlucky", "unpleasant", "upset", "unsatisfactory", "unsightly", "untoward", "unwanted", "unwelcome", "unwholesome", "unwieldy", "unwise", "upset", "vice", "vicious", "vile", "villainous", "vindictive", "wary", "weary", "wicked", "woeful", "worthless", "wound", "yell", "yucky"};
    
    struct timespec ts,te;
    clock_t id = CLOCK_MONOTONIC;
    char buf[102400]={'\0'};
    char send_buf[102400]={'\0'};
    int fd1;
    const char* host_to_docker_path = "/mnt/ramdisk/host_to_docker";
    const char* docker_to_host_path = "/mnt/ramdisk/docker_to_host";
    const char* host_to_docker_signal = "/mnt/ramdisk/host_to_docker_signal";
    const char* docker_to_host_signal = "/mnt/ramdisk/docker_to_host_signal";
    const char* end_path = "/mnt/ramdisk/end";
    while(true){
         // 从host_to_docker读取buffer
        read_from_file(host_to_docker_path,host_to_docker_signal, buf, sizeof(buf));
        //printf("Buffer received from Host\n");
        if((fd1 = open(end_path, O_RDONLY)) != -1)
        {
            close(fd1);
            break;
        }
        struct timespec *tc=(struct timespec *)buf;
        clock_gettime(id, &ts);
        
        std::string text((char *)(buf+16));
        auto sentences = split_sentences(text);
        for (const auto& sentence : sentences) {
            auto words = tokenize(sentence);
            for (const auto& word : words) {
                std::string lower_word = word;
                std::transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);
                if (positive_words.find(lower_word) != positive_words.end()) {
                    //std::cout << "Type: positive, Word: " << word << ", Sentence: " << sentence << std::endl;
                    //printf("%s\n",word.c_str());
                    
                } else if (negative_words.find(lower_word) != negative_words.end()) {
                    //std::cout << "Type: negative, Word: " << word.c_str() << ", Sentence: " << sentence.c_str() << std::endl;
                    //printf("%s\n",word.c_str());
                }
            }
        }
        clock_gettime(id, &te);
        int64_t *time=(int64_t *)send_buf;
        time[2]=(ts.tv_sec - tc->tv_sec) * 1000 * 1000 * 1000 + ts.tv_nsec - tc->tv_nsec;//第一次copy时间
        time[0]=(te.tv_sec - ts.tv_sec) * 1000 * 1000 * 1000 + te.tv_nsec - ts.tv_nsec;
        clock_gettime(id, &ts);
        flatbuffers::FlatBufferBuilder builder(81920);
        std::vector<flatbuffers::Offset<String>> string_vector;
        auto pos = builder.CreateString("POS");
        auto neg = builder.CreateString("NEG");
        for (const auto& sentence : sentences) {
            auto words = tokenize(sentence);
            for (const auto& word : words) {
                std::string lower_word = word;
                std::transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);
                if (positive_words.find(lower_word) != positive_words.end()) {
                    //std::cout << "Type: positive, Word: " << word << ", Sentence: " << sentence << std::endl;
                    auto word_str = builder.CreateString(word.c_str());
                    auto sentence_str = builder.CreateString(sentence.c_str());
                    auto pair = CreateString(builder, pos, word_str, sentence_str);
                    string_vector.push_back(pair);
                } else if (negative_words.find(lower_word) != negative_words.end()) {
                    //std::cout << "Type: negative, Word: " << word << ", Sentence: " << sentence << std::endl;
                    auto word_str = builder.CreateString(word.c_str());
                    auto sentence_str = builder.CreateString(sentence.c_str());
                    auto pair = CreateString(builder, neg, word_str, sentence_str);
                    string_vector.push_back(pair);
                }
            }
        }
        auto pairs_vec = builder.CreateVector(string_vector);
        auto pair_array = CreateStringArray(builder, pairs_vec);
        builder.Finish(pair_array);
        uint8_t *return_buf = builder.GetBufferPointer();
        int buflen=builder.GetSize();
        clock_gettime(id, &te);
        time[1]=(te.tv_sec - ts.tv_sec) * 1000 * 1000 * 1000 + te.tv_nsec - ts.tv_nsec;
        struct timespec *td=(struct timespec *)(send_buf+24);
        clock_gettime(id, td);
        memcpy(send_buf+40,return_buf,buflen);
        // 写入buffer到docker_to_host
        write_to_file(docker_to_host_path,docker_to_host_signal, send_buf, buflen+40);
        //printf("Buffer sent to Host.\n");
    }
    printf("can run here!\n");
    close(fd1);
    unlink(end_path);
    
}
}