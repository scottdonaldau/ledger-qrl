// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
#include <gmock/gmock.h>
#include <xmss-alt/wots.h>
#include <xmss-alt/wots_internal.h>
#include <xmss-alt/hash_address.h>
#include <xmss-alt/xmss_common.h>
#include <xmss-alt/algsxmss.h>
#include <xmss-alt/hash.h>
#include <libxmss/parameters.h>
#include <libxmss/wotsp.h>
#include <libxmss/shash.h>
#include <libxmss/adrs.h>
#include <libxmss/xmss.h>
#include <libxmss/nvram.h>
#include <libxmss/xmss_types.h>

namespace {
TEST(XMSS, hash_h_0)
{
    std::vector<uint8_t> data(2*WOTS_N);
    std::vector<uint8_t> out_ledger(WOTS_N);
    std::vector<uint8_t> out_qrllib(WOTS_N);

    std::vector<uint8_t> pub_seed(WOTS_N);

    hashh_t hashh_in{};
    memset(hashh_in.basic.raw, 0, 96);
    hashh_in.basic.adrs.type = HtoNL(SHASH_TYPE_H);
    hashh_in.basic.adrs.trees.ltree = HtoNL(0u);
    hashh_in.basic.adrs.trees.height = HtoNL(0u);
    hashh_in.basic.adrs.trees.index = HtoNL(0u);
    printf("\n");
    shash_h(out_ledger.data(), data.data(), &hashh_in);

    uint32_t ots_addr[8]{};
    setType(ots_addr, SHASH_TYPE_H);
    setLtreeADRS(ots_addr, 0);
    printf("\n");
    hash_h(eHashFunction::SHA2_256, out_qrllib.data(), data.data(), pub_seed.data(), ots_addr, WOTS_N);

    EXPECT_THAT(out_ledger, ::testing::Eq(out_qrllib));
}

TEST(XMSS, hash_h_1)
{
    std::vector<uint8_t> data(2*WOTS_N);
    std::vector<uint8_t> out_ledger(WOTS_N);
    std::vector<uint8_t> out_qrllib(WOTS_N);

    std::vector<uint8_t> pub_seed(WOTS_N);

    hashh_t hashh_in{};
    memset(hashh_in.basic.raw, 0, 96);
    hashh_in.basic.adrs.type = HtoNL(SHASH_TYPE_H);
    hashh_in.basic.adrs.trees.ltree = HtoNL(1u);
    hashh_in.basic.adrs.trees.height = HtoNL(0u);
    hashh_in.basic.adrs.trees.index = HtoNL(0u);
    printf("\n");
    shash_h(out_ledger.data(), data.data(), &hashh_in);

    uint32_t ots_addr[8]{};
    setType(ots_addr, SHASH_TYPE_H);
    setLtreeADRS(ots_addr, 1);
    printf("\n");
    hash_h(eHashFunction::SHA2_256, out_qrllib.data(), data.data(), pub_seed.data(), ots_addr, WOTS_N);

    EXPECT_THAT(out_ledger, ::testing::Eq(out_qrllib));
}

TEST(XMSS, ltree_gen_0)
{
    std::vector<uint8_t> leaf_ledger(WOTS_N);
    std::vector<uint8_t> leaf_qrllib(WOTS_N);

    std::vector<uint8_t> pk_1(WOTS_N*WOTS_LEN);
    std::vector<uint8_t> pk_2(WOTS_N*WOTS_LEN);

    std::vector<uint8_t> sk(WOTS_N);
    std::vector<uint8_t> pub_seed(WOTS_N);

    uint8_t wots_index = 0;
    wotsp_gen_pk(pk_1.data(), sk.data(), pub_seed.data(), wots_index);
    wotsp_gen_pk(pk_2.data(), sk.data(), pub_seed.data(), wots_index);

    xmss_ltree_gen(leaf_ledger.data(), pk_1.data(), pub_seed.data(), wots_index);

    wots_params params{};
    wots_set_params(&params, WOTS_N, WOTS_W);
    uint32_t ots_addr[8]{};
    setType(ots_addr, 1);
    setLtreeADRS(ots_addr, 0);

    l_tree(eHashFunction::SHA2_256,
            &params,
            leaf_qrllib.data(),
            pk_2.data(),
            pub_seed.data(),
            ots_addr);

    EXPECT_THAT(leaf_ledger, ::testing::Eq(leaf_qrllib));
}

TEST(XMSS, ltree_gen_1)
{
    std::vector<uint8_t> leaf_ledger(WOTS_N);
    std::vector<uint8_t> leaf_qrllib(WOTS_N);

    std::vector<uint8_t> pk_1(WOTS_N*WOTS_LEN);
    std::vector<uint8_t> pk_2(WOTS_N*WOTS_LEN);

    std::vector<uint8_t> sk(WOTS_N);
    std::vector<uint8_t> pub_seed(WOTS_N);
    for (uint8_t i=0; i<32u; i++)
        pub_seed[i]= i<<1u;

    uint8_t wots_index = 0;
    wotsp_gen_pk(pk_1.data(), sk.data(), pub_seed.data(), wots_index);
    wotsp_gen_pk(pk_2.data(), sk.data(), pub_seed.data(), wots_index);

    xmss_ltree_gen(leaf_ledger.data(), pk_1.data(), pub_seed.data(), wots_index);

    wots_params params{};
    wots_set_params(&params, WOTS_N, WOTS_W);
    uint32_t ots_addr[8]{};
    setType(ots_addr, 1);
    setLtreeADRS(ots_addr, 0);

    l_tree(eHashFunction::SHA2_256,
            &params,
            leaf_qrllib.data(),
            pk_2.data(),
            pub_seed.data(),
            ots_addr);

    EXPECT_THAT(leaf_ledger, ::testing::Eq(leaf_qrllib));
}

TEST(XMSS, gen_pk_phase_1) {
    std::vector<uint8_t> sk_seed(48);

    xmss_pk_t pk_1{};
    xmss_pk_t pk_2{};
    xmss_sk_t sk_1{};
    xmss_sk_t sk_2{};

    xmss_gen_keys_1_get_seeds(&sk_1, sk_seed.data());
    xmss_pk(&pk_1, &sk_1);

    xmss_params params{};
    xmss_set_params(&params, WOTS_N, XMSS_H, WOTS_W, XMSS_K);
    xmss_Genkeypair(eHashFunction::SHA2_256,
            &params,
            pk_2.raw,
            sk_2.raw,
            sk_seed.data());

    dump_hex("", sk_1.seed, 32);
    dump_hex("", sk_1.prf_seed, 32);
    dump_hex("", sk_1.pub_seed, 32);

    for(int i=0; i<32; i++)
        ASSERT_EQ(pk_1.pub_seed[i], pk_2.pub_seed[i]);

    for(int i=0; i<32; i++)
        ASSERT_EQ(sk_1.pub_seed[i], sk_2.pub_seed[i]);

    for(int i=0; i<96; i++)
        ASSERT_EQ(sk_1.seeds.raw[i], sk_2.seeds.raw[i]);
}

TEST(XMSS, gen_pk_phase_2_zeroseed_many) {
    std::vector<uint8_t> sk_seed(48);
    xmss_sk_t sk_1{};
    uint8_t xmss_node_ledger[32];
    uint8_t xmss_node_qrllib[32];

    xmss_gen_keys_1_get_seeds(&sk_1, sk_seed.data());

    std::cout << std::endl;

    for (uint16_t idx=0; idx<256; idx+=1)
    {
        xmss_gen_keys_2_get_nodes(xmss_node_ledger,&sk_1, idx);

        ////
        xmss_params params{};
        xmss_set_params(&params, WOTS_N, XMSS_H, WOTS_W, XMSS_K);

        uint32_t ots_addr[8]{};
        uint32_t ltree_addr[8]{};
        setType(ltree_addr, 1);
        setLtreeADRS(ltree_addr, idx);
        setOTSADRS(ots_addr, idx);

        gen_leaf_wots(
                eHashFunction::SHA2_256,
                xmss_node_qrllib,
                sk_1.seed,
                &params,
                sk_1.pub_seed,
                ltree_addr,
                ots_addr);

        dump_hex("", xmss_node_ledger, 32);

        for(int i=0; i<32; i++)
            ASSERT_EQ(xmss_node_ledger[i], xmss_node_qrllib[i]);
    }
}

TEST(XMSS, gen_pk_zeros) {
    std::vector<uint8_t> sk_seed(48);

    xmss_pk_t pk_1{};
    xmss_pk_t pk_2{};
    xmss_sk_t sk_1{};
    xmss_sk_t sk_2{};

    std::cout << std::endl;

    xmss_gen_keys(&sk_1, sk_seed.data());
    xmss_pk(&pk_1, &sk_1 );

    xmss_params params{};
    xmss_set_params(&params, WOTS_N, XMSS_H, WOTS_W, XMSS_K);
    xmss_Genkeypair(eHashFunction::SHA2_256,
            &params,
            pk_2.raw,
            sk_2.raw,
            sk_seed.data());

    dump_hex("Tx: ",pk_1.root, 32);
    dump_hex("Tx: ",pk_2.root, 32);

    for(int i=0; i<64; i++)
    {
        ASSERT_EQ(pk_1.raw[i], pk_2.raw[i]);
    }

    for(int i=0; i<132; i++)
    {
        ASSERT_EQ(sk_1.raw[i], sk_2.raw[i]);
    }
}

TEST(XMSS, gen_pk_somekey) {
    std::vector<uint8_t> sk_seed(48);

    for (uint8_t i=0; i<48u; i++)
        sk_seed[i]= i<<1u;

    xmss_pk_t pk_1{};
    xmss_pk_t pk_2{};
    xmss_sk_t sk_1{};
    xmss_sk_t sk_2{};

    std::cout << std::endl;

    xmss_gen_keys(&sk_1, sk_seed.data());
    xmss_pk(&pk_1, &sk_1);

    xmss_params params{};
    xmss_set_params(&params, WOTS_N, XMSS_H, WOTS_W, XMSS_K);
    xmss_Genkeypair(eHashFunction::SHA2_256,
            &params,
            pk_2.raw,
            sk_2.raw,
            sk_seed.data());

    dump_hex("Tx: ",pk_1.root, 32);
    dump_hex("Tx: ",pk_2.root, 32);

    for(int i=0; i<64; i++)
    {
        ASSERT_EQ(pk_1.raw[i], pk_2.raw[i]);
    }

    for(int i=0; i<132; i++)
    {
        ASSERT_EQ(sk_1.raw[i], sk_2.raw[i]);
    }
}

// FIXME: Move to parameterized tests

TEST(XMSS, sign_idx) {
    const std::vector<uint8_t> sk_seed(SZ_SKSEED);      // This should be coming from the SDK

    const std::vector<uint8_t> msg(32);
    const uint8_t index = 5;

    std::cout << std::endl;

    xmss_gen_keys(&N_DATA.sk, sk_seed.data());
    for (uint16_t idx = 0; idx<XMSS_NUM_NODES; idx++) {
        xmss_gen_keys_2_get_nodes(N_DATA.xmss_nodes+idx*WOTS_N, &N_DATA.sk, idx);
    }

    xmss_signature_t sig_ledger;
    xmss_sign(&sig_ledger, msg.data(), &N_DATA.sk, N_DATA.xmss_nodes, index);

    dump_hex("LEDGER:", sig_ledger.randomness, 32);
    dump_hex("LEDGER:", sig_ledger.wots_sig, WOTS_SIGSIZE);
    dump_hex("LEDGER:", sig_ledger.auth_path, XMSS_AUTHPATHSIZE);

    std::cout << std::endl;

    xmss_signature_t sig_qrllib;

    xmss_params params{};
    xmss_set_params(&params, XMSS_N, XMSS_H, XMSS_W, XMSS_K);

    nvset(&N_DATA.sk.index, NtoHL(index));

    xmss_Signmsg(eHashFunction::SHA2_256,
            &params,
            N_DATA.sk.raw,
            sig_qrllib.raw,
            (uint8_t *) msg.data(), 32);

    dump_hex("QRLLIB:", sig_qrllib.randomness, 32);
    dump_hex("QRLLIB:", sig_qrllib.wots_sig, WOTS_SIGSIZE);
    dump_hex("QRLLIB:", sig_qrllib.auth_path, XMSS_AUTHPATHSIZE);

    ASSERT_EQ(sig_ledger.index, sig_qrllib.index);

    for(int i=0; i<32; i++)
    {
        ASSERT_EQ(sig_ledger.randomness[i], sig_qrllib.randomness[i]);
    }
    for(int i=0; i<WOTS_SIGSIZE; i++)
    {
        ASSERT_EQ(sig_ledger.wots_sig[i], sig_qrllib.wots_sig[i]);
    }
    for(int i=0; i<XMSS_AUTHPATHSIZE; i++)
    {
        ASSERT_EQ(sig_ledger.auth_path[i], sig_qrllib.auth_path[i]);
    }
}

}
