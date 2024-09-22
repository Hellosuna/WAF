#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <sstream>
#include<wasi/api.h>
#include "/home/xiaqian/wasm-udf/performance/flatbuffer/include/str_array_generated.h"
using namespace std;
// 分句函数
extern "C" {
#include <string.h>
typedef struct emo{
    char judge[5];
    char word[50];
    char sentence[3000];
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

// 定义正面和反面词汇
const std::unordered_set<std::string> positive_words = {"adorable", "accepted", "achievement", "active", "admire", "adventure", "affirmative", "affluent", "agree", "agreeable", "amazing", "angelic", "appealing", "approve", "aptitude", "attractive", "awesome", "beaming", "beautiful", "believe", "beneficial", "bliss", "bountiful", "bounty", "brave", "bravo", "brilliant", "celebrated", "champ", "champion", "charming", "cheery", "classic", "classical", "clean", "commend", "composed", "congratulation", "constant", "cool", "courageous", "creative", "cute", "dazzling", "delight", "delightful", "distinguished", "divine", "earnest", "easy", "ecstatic", "effective", "effervescent", "efficient", "effortless", "electrifying", "elegant", "enchanting", "encouraging", "endorsed", "energetic", "energized", "engaging", "enthusiastic", "essential", "esteemed", "ethical", "excellent", "exciting", "exquisite", "fabulous", "fair", "familiar", "famous", "fantastic", "favorable", "fetching", "fine", "fitting", "flourishing", "fortunate", "free", "fresh", "friendly", "fun", "funny", "generous", "genius", "genuine", "giving", "glamorous", "glowing", "good", "gorgeous", "graceful", "great", "green", "grin", "growing", "handsome", "happy", "harmonious", "healing", "healthy", "hearty", "heavenly", "honest", "honorable", "honored", "ideal", "imaginative", "imagine", "impressive", "independent", "innovate", "innovative", "instant", "instantaneous", "instinctive", "intuitive", "intellectual", "intelligent", "inventive", "jovial", "joy", "jubilant", "keen", "kind", "knowing", "knowledgeable", "laugh", "legendary", "light", "learned", "lively", "lovely", "lucid", "lucky", "luminous", "marvelous", "masterful", "meaningful", "merit", "meritorious", "miraculous", "motivating", "moving", "natural", "nice", "novel", "nurturing", "nutritious", "okay", "open", "optimistic", "paradise", "perfect", "phenomenal", "pleasurable", "plentiful", "pleasant", "poised", "polished", "popular", "positive", "powerful", "prepared", "pretty", "principled", "productive", "progress", "prominent", "protected", "proud", "quality", "quick", "quiet", "ready", "reassuring", "refined", "refreshing", "rejoice", "reliable", "remarkable", "resounding", "respected", "restored", "reward", "rewarding", "right", "robust", "safe", "satisfactory", "secure", "seemly", "simple", "skilled", "skillful", "smile", "soulful", "sparkling", "special", "spirited", "spiritual", "stirring", "stupendous", "stunning", "success", "successful", "sunny", "super", "superb", "supporting", "surprising", "terrific", "thorough", "thrilling", "thriving", "tops", "tranquil", "transforming", "transformative", "trusting", "truthful", "unreal", "unwavering", "upbeat", "upright", "upstanding", "valued", "vibrant", "victorious", "victory", "vigorous", "virtuous", "vital", "vivacious", "wealthy", "welcome", "well", "whole", "wholesome", "willing", "wonderful", "wondrous", "worthy", "wow", "yummy", "zeal", "zealous"};
const std::unordered_set<std::string> negative_words = {"abysmal", "adverse", "alarming", "angry", "annoy", "anxious", "apathy", "appalling", "atrocious", "awful", "bad", "banal", "barbed", "belligerent", "bemoan", "beneath", "boring", "broken", "callous", "clumsy", "coarse", "cold", "collapse", "confused", "contradictory", "contrary", "corrosive", "corrupt", "crazy", "creepy", "criminal", "cruel", "cry", "cutting", "decaying", "damage", "damaging", "dastardly", "deplorable", "depressed", "deprived", "deformed", "deny", "despicable", "detrimental", "dirty", "disease", "disgusting", "disheveled", "dishonest", "dishonorable", "dismal", "distress", "dreadful", "dreary", "enraged", "eroding", "evil", "fail", "faulty", "feeble", "fight", "filthy", "foul", "frighten", "frightful", "gawky", "ghastly", "grave", "greed", "grim", "grimace", "gross", "grotesque", "gruesome", "guilty", "haggard", "hard", "hard-hearted", "harmful", "hate", "hideous", "homely", "horrendous", "horrible", "hostile", "hurt", "hurtful", "icky", "ignore", "ignorant", "ill", "immature", "imperfect", "impossible", "inane", "inelegant", "infernal", "injure", "injurious", "insane", "insidious", "insipid", "jealous", "junky", "lose", "lousy", "lumpy", "malicious", "mean", "menacing", "messy", "misshapen", "missing", "misunderstood", "moan", "moldy", "monstrous", "naive", "nasty", "naughty", "negate", "negative", "never", "nobody", "nondescript", "nonsense", "noxious", "objectionable", "odious", "offensive", "old", "oppressive", "pain", "perturb", "pessimistic", "petty", "plain", "poisonous", "poor", "prejudice", "questionable", "quirky", "quit", "reject", "renege", "repellant", "reptilian", "repulsive", "repugnant", "revenge", "revolting", "rocky", "rotten", "rude", "ruthless", "sad", "savage", "scare", "scary", "scream", "severe", "shoddy", "shocking", "sick", "sickening", "sinister", "slimy", "smelly", "sobbing", "sorry", "spiteful", "sticky", "stinky", "stormy", "stressful", "stuck", "stupid", "substandard", "suspect", "suspicious", "tense", "terrible", "terrifying", "threatening", "ugly", "undermine", "unfair", "unfavorable", "unhappy", "unhealthy", "unjust", "unlucky", "unpleasant", "upset", "unsatisfactory", "unsightly", "untoward", "unwanted", "unwelcome", "unwholesome", "unwieldy", "unwise", "upset", "vice", "vicious", "vile", "villainous", "vindictive", "wary", "weary", "wicked", "woeful", "worthless", "wound", "yell", "yucky"};

void* extract_sentiment_with_ser(void* addr,void* buf2,uint64_t time_begin) {
    __wasi_timestamp_t tc,ts,te;
    tc=(__wasi_timestamp_t)time_begin;
    int64_t *time=(int64_t *)buf2;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    const std::unordered_set<std::string> positive_words = {"adorable", "accepted", "achievement", "active", "admire", "adventure", "affirmative", "affluent", "agree", "agreeable", "amazing", "angelic", "appealing", "approve", "aptitude", "attractive", "awesome", "beaming", "beautiful", "believe", "beneficial", "bliss", "bountiful", "bounty", "brave", "bravo", "brilliant", "celebrated", "champ", "champion", "charming", "cheery", "classic", "classical", "clean", "commend", "composed", "congratulation", "constant", "cool", "courageous", "creative", "cute", "dazzling", "delight", "delightful", "distinguished", "divine", "earnest", "easy", "ecstatic", "effective", "effervescent", "efficient", "effortless", "electrifying", "elegant", "enchanting", "encouraging", "endorsed", "energetic", "energized", "engaging", "enthusiastic", "essential", "esteemed", "ethical", "excellent", "exciting", "exquisite", "fabulous", "fair", "familiar", "famous", "fantastic", "favorable", "fetching", "fine", "fitting", "flourishing", "fortunate", "free", "fresh", "friendly", "fun", "funny", "generous", "genius", "genuine", "giving", "glamorous", "glowing", "good", "gorgeous", "graceful", "great", "green", "grin", "growing", "handsome", "happy", "harmonious", "healing", "healthy", "hearty", "heavenly", "honest", "honorable", "honored", "ideal", "imaginative", "imagine", "impressive", "independent", "innovate", "innovative", "instant", "instantaneous", "instinctive", "intuitive", "intellectual", "intelligent", "inventive", "jovial", "joy", "jubilant", "keen", "kind", "knowing", "knowledgeable", "laugh", "legendary", "light", "learned", "lively", "lovely", "lucid", "lucky", "luminous", "marvelous", "masterful", "meaningful", "merit", "meritorious", "miraculous", "motivating", "moving", "natural", "nice", "novel", "nurturing", "nutritious", "okay", "open", "optimistic", "paradise", "perfect", "phenomenal", "pleasurable", "plentiful", "pleasant", "poised", "polished", "popular", "positive", "powerful", "prepared", "pretty", "principled", "productive", "progress", "prominent", "protected", "proud", "quality", "quick", "quiet", "ready", "reassuring", "refined", "refreshing", "rejoice", "reliable", "remarkable", "resounding", "respected", "restored", "reward", "rewarding", "right", "robust", "safe", "satisfactory", "secure", "seemly", "simple", "skilled", "skillful", "smile", "soulful", "sparkling", "special", "spirited", "spiritual", "stirring", "stupendous", "stunning", "success", "successful", "sunny", "super", "superb", "supporting", "surprising", "terrific", "thorough", "thrilling", "thriving", "tops", "tranquil", "transforming", "transformative", "trusting", "truthful", "unreal", "unwavering", "upbeat", "upright", "upstanding", "valued", "vibrant", "victorious", "victory", "vigorous", "virtuous", "vital", "vivacious", "wealthy", "welcome", "well", "whole", "wholesome", "willing", "wonderful", "wondrous", "worthy", "wow", "yummy", "zeal", "zealous"};
    const std::unordered_set<std::string> negative_words = {"abysmal", "adverse", "alarming", "angry", "annoy", "anxious", "apathy", "appalling", "atrocious", "awful", "bad", "banal", "barbed", "belligerent", "bemoan", "beneath", "boring", "broken", "callous", "clumsy", "coarse", "cold", "collapse", "confused", "contradictory", "contrary", "corrosive", "corrupt", "crazy", "creepy", "criminal", "cruel", "cry", "cutting", "decaying", "damage", "damaging", "dastardly", "deplorable", "depressed", "deprived", "deformed", "deny", "despicable", "detrimental", "dirty", "disease", "disgusting", "disheveled", "dishonest", "dishonorable", "dismal", "distress", "dreadful", "dreary", "enraged", "eroding", "evil", "fail", "faulty", "feeble", "fight", "filthy", "foul", "frighten", "frightful", "gawky", "ghastly", "grave", "greed", "grim", "grimace", "gross", "grotesque", "gruesome", "guilty", "haggard", "hard", "hard-hearted", "harmful", "hate", "hideous", "homely", "horrendous", "horrible", "hostile", "hurt", "hurtful", "icky", "ignore", "ignorant", "ill", "immature", "imperfect", "impossible", "inane", "inelegant", "infernal", "injure", "injurious", "insane", "insidious", "insipid", "jealous", "junky", "lose", "lousy", "lumpy", "malicious", "mean", "menacing", "messy", "misshapen", "missing", "misunderstood", "moan", "moldy", "monstrous", "naive", "nasty", "naughty", "negate", "negative", "never", "nobody", "nondescript", "nonsense", "noxious", "objectionable", "odious", "offensive", "old", "oppressive", "pain", "perturb", "pessimistic", "petty", "plain", "poisonous", "poor", "prejudice", "questionable", "quirky", "quit", "reject", "renege", "repellant", "reptilian", "repulsive", "repugnant", "revenge", "revolting", "rocky", "rotten", "rude", "ruthless", "sad", "savage", "scare", "scary", "scream", "severe", "shoddy", "shocking", "sick", "sickening", "sinister", "slimy", "smelly", "sobbing", "sorry", "spiteful", "sticky", "stinky", "stormy", "stressful", "stuck", "stupid", "substandard", "suspect", "suspicious", "tense", "terrible", "terrifying", "threatening", "ugly", "undermine", "unfair", "unfavorable", "unhappy", "unhealthy", "unjust", "unlucky", "unpleasant", "upset", "unsatisfactory", "unsightly", "untoward", "unwanted", "unwelcome", "unwholesome", "unwieldy", "unwise", "upset", "vice", "vicious", "vile", "villainous", "vindictive", "wary", "weary", "wicked", "woeful", "worthless", "wound", "yell", "yucky"};
    
    std::string text((char *)addr);
    auto sentences = split_sentences(text);
    for (const auto& sentence : sentences) {
        auto words = tokenize(sentence);
        for (const auto& word : words) {
            std::string lower_word = word;
            std::transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);
            if (positive_words.find(lower_word) != positive_words.end()) {
                //std::cout << "Type: positive, Word: " << word << ", Sentence: " << sentence << std::endl;
                printf("%s\n",word.c_str());
                
            } else if (negative_words.find(lower_word) != negative_words.end()) {
                //std::cout << "Type: negative, Word: " << word.c_str() << ", Sentence: " << sentence.c_str() << std::endl;
                printf("%s\n",word.c_str());
            }
        }
    }
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    time[0]=(int64_t)(te-ts);
    time[2]=(int64_t)(ts-tc);
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    flatbuffers::FlatBufferBuilder builder(8192);
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
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    time[1]=(int64_t)(te-ts);
    time[3]=(int64_t)te;
    return builder.GetBufferPointer();
}

void* extract_sentiment(void* addr,void* buf2,uint64_t time_begin) {
    __wasi_timestamp_t tc,ts,te;
    tc=( __wasi_timestamp_t)time_begin;
    int64_t *time=(int64_t *)buf2;
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &ts);
    const std::unordered_set<std::string> positive_words = {"adorable", "accepted", "achievement", "active", "admire", "adventure", "affirmative", "affluent", "agree", "agreeable", "amazing", "angelic", "appealing", "approve", "aptitude", "attractive", "awesome", "beaming", "beautiful", "believe", "beneficial", "bliss", "bountiful", "bounty", "brave", "bravo", "brilliant", "celebrated", "champ", "champion", "charming", "cheery", "classic", "classical", "clean", "commend", "composed", "congratulation", "constant", "cool", "courageous", "creative", "cute", "dazzling", "delight", "delightful", "distinguished", "divine", "earnest", "easy", "ecstatic", "effective", "effervescent", "efficient", "effortless", "electrifying", "elegant", "enchanting", "encouraging", "endorsed", "energetic", "energized", "engaging", "enthusiastic", "essential", "esteemed", "ethical", "excellent", "exciting", "exquisite", "fabulous", "fair", "familiar", "famous", "fantastic", "favorable", "fetching", "fine", "fitting", "flourishing", "fortunate", "free", "fresh", "friendly", "fun", "funny", "generous", "genius", "genuine", "giving", "glamorous", "glowing", "good", "gorgeous", "graceful", "great", "green", "grin", "growing", "handsome", "happy", "harmonious", "healing", "healthy", "hearty", "heavenly", "honest", "honorable", "honored", "ideal", "imaginative", "imagine", "impressive", "independent", "innovate", "innovative", "instant", "instantaneous", "instinctive", "intuitive", "intellectual", "intelligent", "inventive", "jovial", "joy", "jubilant", "keen", "kind", "knowing", "knowledgeable", "laugh", "legendary", "light", "learned", "lively", "lovely", "lucid", "lucky", "luminous", "marvelous", "masterful", "meaningful", "merit", "meritorious", "miraculous", "motivating", "moving", "natural", "nice", "novel", "nurturing", "nutritious", "okay", "open", "optimistic", "paradise", "perfect", "phenomenal", "pleasurable", "plentiful", "pleasant", "poised", "polished", "popular", "positive", "powerful", "prepared", "pretty", "principled", "productive", "progress", "prominent", "protected", "proud", "quality", "quick", "quiet", "ready", "reassuring", "refined", "refreshing", "rejoice", "reliable", "remarkable", "resounding", "respected", "restored", "reward", "rewarding", "right", "robust", "safe", "satisfactory", "secure", "seemly", "simple", "skilled", "skillful", "smile", "soulful", "sparkling", "special", "spirited", "spiritual", "stirring", "stupendous", "stunning", "success", "successful", "sunny", "super", "superb", "supporting", "surprising", "terrific", "thorough", "thrilling", "thriving", "tops", "tranquil", "transforming", "transformative", "trusting", "truthful", "unreal", "unwavering", "upbeat", "upright", "upstanding", "valued", "vibrant", "victorious", "victory", "vigorous", "virtuous", "vital", "vivacious", "wealthy", "welcome", "well", "whole", "wholesome", "willing", "wonderful", "wondrous", "worthy", "wow", "yummy", "zeal", "zealous"};
    const std::unordered_set<std::string> negative_words = {"abysmal", "adverse", "alarming", "angry", "annoy", "anxious", "apathy", "appalling", "atrocious", "awful", "bad", "banal", "barbed", "belligerent", "bemoan", "beneath", "boring", "broken", "callous", "clumsy", "coarse", "cold", "collapse", "confused", "contradictory", "contrary", "corrosive", "corrupt", "crazy", "creepy", "criminal", "cruel", "cry", "cutting", "decaying", "damage", "damaging", "dastardly", "deplorable", "depressed", "deprived", "deformed", "deny", "despicable", "detrimental", "dirty", "disease", "disgusting", "disheveled", "dishonest", "dishonorable", "dismal", "distress", "dreadful", "dreary", "enraged", "eroding", "evil", "fail", "faulty", "feeble", "fight", "filthy", "foul", "frighten", "frightful", "gawky", "ghastly", "grave", "greed", "grim", "grimace", "gross", "grotesque", "gruesome", "guilty", "haggard", "hard", "hard-hearted", "harmful", "hate", "hideous", "homely", "horrendous", "horrible", "hostile", "hurt", "hurtful", "icky", "ignore", "ignorant", "ill", "immature", "imperfect", "impossible", "inane", "inelegant", "infernal", "injure", "injurious", "insane", "insidious", "insipid", "jealous", "junky", "lose", "lousy", "lumpy", "malicious", "mean", "menacing", "messy", "misshapen", "missing", "misunderstood", "moan", "moldy", "monstrous", "naive", "nasty", "naughty", "negate", "negative", "never", "nobody", "nondescript", "nonsense", "noxious", "objectionable", "odious", "offensive", "old", "oppressive", "pain", "perturb", "pessimistic", "petty", "plain", "poisonous", "poor", "prejudice", "questionable", "quirky", "quit", "reject", "renege", "repellant", "reptilian", "repulsive", "repugnant", "revenge", "revolting", "rocky", "rotten", "rude", "ruthless", "sad", "savage", "scare", "scary", "scream", "severe", "shoddy", "shocking", "sick", "sickening", "sinister", "slimy", "smelly", "sobbing", "sorry", "spiteful", "sticky", "stinky", "stormy", "stressful", "stuck", "stupid", "substandard", "suspect", "suspicious", "tense", "terrible", "terrifying", "threatening", "ugly", "undermine", "unfair", "unfavorable", "unhappy", "unhealthy", "unjust", "unlucky", "unpleasant", "upset", "unsatisfactory", "unsightly", "untoward", "unwanted", "unwelcome", "unwholesome", "unwieldy", "unwise", "upset", "vice", "vicious", "vile", "villainous", "vindictive", "wary", "weary", "wicked", "woeful", "worthless", "wound", "yell", "yucky"};
    std::string text((char *)addr);
    auto sentences = split_sentences(text);
    emo results[100];//假设最多100个情感词
    int64_t len=0;
    for (const auto& sentence : sentences) {
        auto words = tokenize(sentence);
        for (const auto& word : words) {
            std::string lower_word = word;
            std::transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);
            if (positive_words.find(lower_word) != positive_words.end()) {
                //std::cout << "Type: positive, Word: " << word << ", Sentence: " << sentence << std::endl;
                strncpy(results[len].judge, "POS", 3);
                results[len].judge[3]='\0';
                //std::cout<<word.c_str()<<word.length()<<"  "<<sentence.c_str()<<sentence.length()<<std::endl;
                strncpy(results[len].word, word.c_str(), word.length());
                results[len].word[word.length()]='\0';
                strncpy(results[len].sentence, sentence.c_str(), sentence.length());
                results[len].sentence[sentence.length()]='\0';
                len++;
                
            } else if (negative_words.find(lower_word) != negative_words.end()) {
                //std::cout << "Type: negative, Word: " << word.c_str() << ", Sentence: " << sentence.c_str() << std::endl;
                strncpy(results[len].judge, "NEG", 3);
                results[len].judge[3]='\0';
                //std::cout<<word.c_str()<<sizeof(word.c_str())<<"  "<<sentence.c_str()<<sizeof(sentence.c_str())<<std::endl;
                strncpy(results[len].word, word.c_str(),word.length());
                results[len].word[word.length()]='\0';
                strncpy(results[len].sentence, sentence.c_str(), sentence.length());
                results[len].sentence[sentence.length()]='\0';
                len++;
            }
        }
    }
    __wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 1, &te);
    time[0]=(int64_t)(te-ts);
    time[1]=len;
    time[2]=(int64_t)(ts-tc);//第一次拷贝时间
    time[3]=(int64_t)te;
    return (void *)results;//注意时间下标不同
}


/*int main() {
    char *str1 = "I has a Pen! It looks great.";
    int64_t arr[2];
    void *result1 = extract_sentiment_with_ser(str1,arr);
    const StringArray *string_array = GetStringArray(result1);
    auto value = string_array->arrays();
    auto len = value->size();
    for (int i = 0; i < len; i++) {
        std::cout << (value->Get(i)->sentiment())->c_str() << " "
                  << value->Get(i)->word()->c_str() << " "
                  << value->Get(i)->sentence()->c_str() << std::endl;
    }
    char *str2 = "This is a bad product! I love it. The quality is bad.";
    int64_t len[2];
    emo* result2=(emo*)extract_sentiment(str2,len);
    
    for(int i=0;i<len[1];i++)
    {
           std::cout << "Judge: " << result2[i].judge << ", Word: " << result2[i].word << ", Sentence: " << result2[i].sentence << std::endl;
    }
    return 0;
}*/
}
