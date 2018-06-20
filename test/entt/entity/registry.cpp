#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <type_traits>
#include <gtest/gtest.h>
#include <entt/entity/entt_traits.hpp>
#include <entt/entity/registry.hpp>

struct Listener {
    template<typename Component>
    void incrComponent(entt::Registry<> &registry, entt::Registry<>::entity_type entity) {
        ASSERT_TRUE(registry.valid(entity));
        ASSERT_TRUE(registry.has<Component>(entity));
        last = entity;
        ++counter;
    }

    template<typename Tag>
    void incrTag(entt::Registry<> &registry, entt::Registry<>::entity_type entity) {
        ASSERT_TRUE(registry.valid(entity));
        ASSERT_TRUE(registry.has<Tag>());
        ASSERT_EQ(registry.attachee<Tag>(), entity);
        last = entity;
        ++counter;
    }

    template<typename Component>
    void decrComponent(entt::Registry<> &registry, entt::Registry<>::entity_type entity) {
        ASSERT_TRUE(registry.valid(entity));
        ASSERT_TRUE(registry.has<Component>(entity));
        last = entity;
        --counter;
    }

    template<typename Tag>
    void decrTag(entt::Registry<> &registry, entt::Registry<>::entity_type entity) {
        ASSERT_TRUE(registry.valid(entity));
        ASSERT_TRUE(registry.has<Tag>());
        ASSERT_EQ(registry.attachee<Tag>(), entity);
        last = entity;
        --counter;
    }

    entt::Registry<>::entity_type last;
    int counter{0};
};

TEST(Registry, Types) {
    entt::Registry registry;

    ASSERT_EQ(registry.type<int>(entt::tag_t{}), registry.type<int>(entt::tag_t{}));
    ASSERT_EQ(registry.type<int>(), registry.type<int>());

    ASSERT_NE(registry.type<int>(entt::tag_t{}), registry.type<double>(entt::tag_t{}));
    ASSERT_NE(registry.type<int>(), registry.type<double>(entt::tag_t{}));
}

TEST(Registry, Functionalities) {
    entt::Registry registry;

    ASSERT_EQ(registry.size(), entt::Registry<>::size_type{0});
    ASSERT_NO_THROW(registry.reserve(42));
    ASSERT_NO_THROW(registry.reserve<int>(8));
    ASSERT_NO_THROW(registry.reserve<char>(8));
    ASSERT_TRUE(registry.empty());

    ASSERT_EQ(registry.capacity(), entt::Registry<>::size_type{0});
    ASSERT_EQ(registry.capacity<int>(), entt::Registry<>::size_type{8});
    ASSERT_EQ(registry.capacity<char>(), entt::Registry<>::size_type{8});
    ASSERT_EQ(registry.size<int>(), entt::Registry<>::size_type{0});
    ASSERT_EQ(registry.size<char>(), entt::Registry<>::size_type{0});
    ASSERT_TRUE(registry.empty<int>());
    ASSERT_TRUE(registry.empty<char>());

    const auto e0 = registry.create();
    const auto e1 = registry.create();

    registry.assign<int>(e1);
    registry.assign<char>(e1);

    ASSERT_TRUE(registry.has<>(e0));
    ASSERT_TRUE(registry.has<>(e1));

    ASSERT_EQ(registry.capacity(), entt::Registry<>::size_type{2});
    ASSERT_EQ(registry.size<int>(), entt::Registry<>::size_type{1});
    ASSERT_EQ(registry.size<char>(), entt::Registry<>::size_type{1});
    ASSERT_FALSE(registry.empty<int>());
    ASSERT_FALSE(registry.empty<char>());

    ASSERT_NE(e0, e1);

    ASSERT_FALSE(registry.has<int>(e0));
    ASSERT_TRUE(registry.has<int>(e1));
    ASSERT_FALSE(registry.has<char>(e0));
    ASSERT_TRUE(registry.has<char>(e1));
    ASSERT_FALSE((registry.has<int, char>(e0)));
    ASSERT_TRUE((registry.has<int, char>(e1)));

    ASSERT_EQ(registry.assign<int>(e0, 42), 42);
    ASSERT_EQ(registry.assign<char>(e0, 'c'), 'c');
    ASSERT_NO_THROW(registry.remove<int>(e1));
    ASSERT_NO_THROW(registry.remove<char>(e1));

    ASSERT_TRUE(registry.has<int>(e0));
    ASSERT_FALSE(registry.has<int>(e1));
    ASSERT_TRUE(registry.has<char>(e0));
    ASSERT_FALSE(registry.has<char>(e1));
    ASSERT_TRUE((registry.has<int, char>(e0)));
    ASSERT_FALSE((registry.has<int, char>(e1)));

    const auto e2 = registry.create();

    registry.accommodate<int>(e2, registry.get<int>(e0));
    registry.accommodate<char>(e2, registry.get<char>(e0));

    ASSERT_TRUE(registry.has<int>(e2));
    ASSERT_TRUE(registry.has<char>(e2));
    ASSERT_EQ(registry.get<int>(e0), 42);
    ASSERT_EQ(registry.get<char>(e0), 'c');

    ASSERT_EQ(std::get<0>(registry.get<int, char>(e0)), 42);
    ASSERT_EQ(std::get<1>(static_cast<const entt::Registry<> &>(registry).get<int, char>(e0)), 'c');

    ASSERT_EQ(registry.get<int>(e0), registry.get<int>(e2));
    ASSERT_EQ(registry.get<char>(e0), registry.get<char>(e2));
    ASSERT_NE(&registry.get<int>(e0), &registry.get<int>(e2));
    ASSERT_NE(&registry.get<char>(e0), &registry.get<char>(e2));

    ASSERT_NO_THROW(registry.replace<int>(e0, 0));
    ASSERT_EQ(registry.get<int>(e0), 0);

    ASSERT_NO_THROW(registry.accommodate<int>(e0, 1));
    ASSERT_NO_THROW(registry.accommodate<int>(e1, 1));
    ASSERT_EQ(static_cast<const entt::Registry<> &>(registry).get<int>(e0), 1);
    ASSERT_EQ(static_cast<const entt::Registry<> &>(registry).get<int>(e1), 1);

    ASSERT_EQ(registry.size(), entt::Registry<>::size_type{3});
    ASSERT_FALSE(registry.empty());

    ASSERT_EQ(registry.version(e2), entt::Registry<>::version_type{0});
    ASSERT_EQ(registry.current(e2), entt::Registry<>::version_type{0});
    ASSERT_NO_THROW(registry.destroy(e2));
    ASSERT_EQ(registry.version(e2), entt::Registry<>::version_type{0});
    ASSERT_EQ(registry.current(e2), entt::Registry<>::version_type{1});

    ASSERT_TRUE(registry.valid(e0));
    ASSERT_TRUE(registry.fast(e0));
    ASSERT_TRUE(registry.valid(e1));
    ASSERT_TRUE(registry.fast(e1));
    ASSERT_FALSE(registry.valid(e2));
    ASSERT_FALSE(registry.fast(e2));

    ASSERT_EQ(registry.size(), entt::Registry<>::size_type{2});
    ASSERT_FALSE(registry.empty());

    ASSERT_NO_THROW(registry.reset());

    ASSERT_EQ(registry.size(), entt::Registry<>::size_type{0});
    ASSERT_TRUE(registry.empty());

    const auto e3 = registry.create();

    registry.assign<int>(e3);
    registry.assign<char>(e3);

    ASSERT_EQ(registry.size<int>(), entt::Registry<>::size_type{1});
    ASSERT_EQ(registry.size<char>(), entt::Registry<>::size_type{1});
    ASSERT_FALSE(registry.empty<int>());
    ASSERT_FALSE(registry.empty<char>());

    ASSERT_NO_THROW(registry.reset<int>());

    ASSERT_EQ(registry.size<int>(), entt::Registry<>::size_type{0});
    ASSERT_EQ(registry.size<char>(), entt::Registry<>::size_type{1});
    ASSERT_TRUE(registry.empty<int>());
    ASSERT_FALSE(registry.empty<char>());

    ASSERT_NO_THROW(registry.reset());

    ASSERT_EQ(registry.size<int>(), entt::Registry<>::size_type{0});
    ASSERT_EQ(registry.size<char>(), entt::Registry<>::size_type{0});
    ASSERT_TRUE(registry.empty<int>());
    ASSERT_TRUE(registry.empty<char>());

    const auto e4 = registry.create();
    const auto e5 = registry.create();

    registry.assign<int>(e4);

    ASSERT_NO_THROW(registry.reset<int>(e4));
    ASSERT_NO_THROW(registry.reset<int>(e5));

    ASSERT_EQ(registry.size<int>(), entt::Registry<>::size_type{0});
    ASSERT_EQ(registry.size<char>(), entt::Registry<>::size_type{0});
    ASSERT_TRUE(registry.empty<int>());
}

TEST(Registry, RawData) {
    entt::Registry registry;
    const entt::Registry<> &cregistry = registry;
    const auto entity = registry.create();

    ASSERT_EQ(registry.raw<int>(), nullptr);
    ASSERT_EQ(cregistry.raw<int>(), nullptr);
    ASSERT_EQ(cregistry.data<int>(), nullptr);

    registry.assign<int>(entity, 42);

    ASSERT_NE(registry.raw<int>(), nullptr);
    ASSERT_NE(cregistry.raw<int>(), nullptr);
    ASSERT_NE(cregistry.data<int>(), nullptr);

    ASSERT_EQ(*registry.raw<int>(), 42);
    ASSERT_EQ(*cregistry.raw<int>(), 42);
    ASSERT_EQ(*cregistry.data<int>(), entity);
}

TEST(Registry, CreateDestroyCornerCase) {
    entt::Registry registry;

    const auto e0 = registry.create();
    const auto e1 = registry.create();

    registry.destroy(e0);
    registry.destroy(e1);

    registry.each([](auto) { FAIL(); });

    ASSERT_EQ(registry.current(e0), entt::Registry<>::version_type{1});
    ASSERT_EQ(registry.current(e1), entt::Registry<>::version_type{1});
}

TEST(Registry, VersionOverflow) {
    entt::Registry registry;

    const auto entity = registry.create();
    registry.destroy(entity);

    ASSERT_EQ(registry.version(entity), entt::Registry<>::version_type{});

    for(auto i = entt::entt_traits<entt::Registry<>::entity_type>::version_mask; i; --i) {
        ASSERT_NE(registry.current(entity), registry.version(entity));
        registry.destroy(registry.create());
    }

    ASSERT_EQ(registry.current(entity), registry.version(entity));
}

TEST(Registry, Each) {
    entt::Registry registry;
    entt::Registry<>::size_type tot;
    entt::Registry<>::size_type match;

    registry.create();
    registry.assign<int>(registry.create());
    registry.create();
    registry.assign<int>(registry.create());
    registry.create();

    tot = 0u;
    match = 0u;

    registry.each([&](auto entity) {
        if(registry.has<int>(entity)) { ++match; }
        registry.create();
        ++tot;
    });

    ASSERT_EQ(tot, 5u);
    ASSERT_EQ(match, 2u);

    tot = 0u;
    match = 0u;

    registry.each([&](auto entity) {
        if(registry.has<int>(entity)) {
            registry.destroy(entity);
            ++match;
        }

        ++tot;
    });

    ASSERT_EQ(tot, 10u);
    ASSERT_EQ(match, 2u);

    tot = 0u;
    match = 0u;

    registry.each([&](auto entity) {
        if(registry.has<int>(entity)) { ++match; }
        registry.destroy(entity);
        ++tot;
    });

    ASSERT_EQ(tot, 8u);
    ASSERT_EQ(match, 0u);

    registry.each([&](auto) { FAIL(); });
}

TEST(Registry, Orphans) {
    entt::Registry registry;
    entt::Registry<>::size_type tot{};

    registry.assign<int>(registry.create());
    registry.create();
    registry.assign<int>(registry.create());
    registry.create();
    registry.assign<double>(entt::tag_t{}, registry.create());

    registry.orphans([&](auto) { ++tot; });
    ASSERT_EQ(tot, 2u);
    tot = 0u;

    registry.each([&](auto entity) { registry.reset<int>(entity); });
    registry.orphans([&](auto) { ++tot; });
    ASSERT_EQ(tot, 4u);
    registry.reset();
    tot = 0u;

    registry.orphans([&](auto) { ++tot; });
    ASSERT_EQ(tot, 0u);
}

TEST(Registry, CreateDestroyEntities) {
    entt::Registry registry;
    entt::Registry<>::entity_type pre{}, post{};

    for(int i = 0; i < 10; ++i) {
        const auto entity = registry.create();
        registry.assign<double>(entity);
    }

    registry.reset();

    for(int i = 0; i < 7; ++i) {
        const auto entity = registry.create();
        registry.assign<int>(entity);
        if(i == 3) { pre = entity; }
    }

    registry.reset();

    for(int i = 0; i < 5; ++i) {
        const auto entity = registry.create();
        if(i == 3) { post = entity; }
    }

    ASSERT_FALSE(registry.valid(pre));
    ASSERT_TRUE(registry.valid(post));
    ASSERT_NE(registry.version(pre), registry.version(post));
    ASSERT_EQ(registry.version(pre) + 1, registry.version(post));
    ASSERT_EQ(registry.current(pre), registry.current(post));
}

TEST(Registry, AttachSetRemoveTags) {
    entt::Registry registry;
    const auto &cregistry = registry;

    ASSERT_FALSE(registry.has<int>());

    const auto entity = registry.create();
    registry.assign<int>(entt::tag_t{}, entity, 42);

    ASSERT_TRUE(registry.has<int>());
    ASSERT_TRUE(registry.has<int>(entt::tag_t{}, entity));
    ASSERT_EQ(registry.get<int>(), 42);
    ASSERT_EQ(cregistry.get<int>(), 42);
    ASSERT_EQ(registry.attachee<int>(), entity);

    registry.replace<int>(entt::tag_t{}, 3);

    ASSERT_TRUE(registry.has<int>());
    ASSERT_TRUE(registry.has<int>(entt::tag_t{}, entity));
    ASSERT_EQ(registry.get<int>(), 3);
    ASSERT_EQ(cregistry.get<int>(), 3);
    ASSERT_EQ(registry.attachee<int>(), entity);

    const auto other = registry.create();
    registry.move<int>(other);

    ASSERT_TRUE(registry.has<int>());
    ASSERT_FALSE(registry.has<int>(entt::tag_t{}, entity));
    ASSERT_TRUE(registry.has<int>(entt::tag_t{}, other));
    ASSERT_EQ(registry.get<int>(), 3);
    ASSERT_EQ(cregistry.get<int>(), 3);
    ASSERT_EQ(registry.attachee<int>(), other);

    registry.remove<int>();

    ASSERT_FALSE(registry.has<int>());
    ASSERT_FALSE(registry.has<int>(entt::tag_t{}, entity));
    ASSERT_FALSE(registry.has<int>(entt::tag_t{}, other));

    registry.assign<int>(entt::tag_t{}, entity, 42);
    registry.destroy(entity);

    ASSERT_FALSE(registry.has<int>());
    ASSERT_FALSE(registry.has<int>(entt::tag_t{}, entity));
    ASSERT_FALSE(registry.has<int>(entt::tag_t{}, other));
}

TEST(Registry, StandardView) {
    entt::Registry registry;
    auto mview = registry.view<int, char>();
    auto iview = registry.view<int>();
    auto cview = registry.view<char>();

    const auto e0 = registry.create();
    registry.assign<int>(e0, 0);
    registry.assign<char>(e0, 'c');

    const auto e1 = registry.create();
    registry.assign<int>(e1, 0);

    const auto e2 = registry.create();
    registry.assign<int>(e2, 0);
    registry.assign<char>(e2, 'c');

    ASSERT_EQ(iview.size(), decltype(iview)::size_type{3});
    ASSERT_EQ(cview.size(), decltype(cview)::size_type{2});

    decltype(mview)::size_type cnt{0};
    mview.each([&cnt](auto...) { ++cnt; });

    ASSERT_EQ(cnt, decltype(mview)::size_type{2});
}

TEST(Registry, PersistentView) {
    entt::Registry registry;
    auto view = registry.view<int, char>(entt::persistent_t{});

    ASSERT_TRUE((registry.contains<int, char>()));
    ASSERT_FALSE((registry.contains<int, double>()));

    registry.prepare<int, double>();

    ASSERT_TRUE((registry.contains<int, double>()));

    registry.discard<int, double>();

    ASSERT_FALSE((registry.contains<int, double>()));

    const auto e0 = registry.create();
    registry.assign<int>(e0, 0);
    registry.assign<char>(e0, 'c');

    const auto e1 = registry.create();
    registry.assign<int>(e1, 0);

    const auto e2 = registry.create();
    registry.assign<int>(e2, 0);
    registry.assign<char>(e2, 'c');

    decltype(view)::size_type cnt{0};
    view.each([&cnt](auto...) { ++cnt; });

    ASSERT_EQ(cnt, decltype(view)::size_type{2});
}

TEST(Registry, RawView) {
    entt::Registry registry;
    auto view = registry.view<int>(entt::raw_t{});

    const auto e0 = registry.create();
    registry.assign<int>(e0, 0);
    registry.assign<char>(e0, 'c');

    const auto e1 = registry.create();
    registry.assign<int>(e1, 0);
    registry.assign<char>(e1, 'c');

    decltype(view)::size_type cnt{0};
    view.each([&cnt](auto &...) { ++cnt; });

    ASSERT_EQ(cnt, decltype(view)::size_type{2});
}

TEST(Registry, CleanStandardViewAfterReset) {
    entt::Registry registry;
    auto view = registry.view<int>();
    registry.assign<int>(registry.create(), 0);

    ASSERT_EQ(view.size(), entt::Registry<>::size_type{1});

    registry.reset();

    ASSERT_EQ(view.size(), entt::Registry<>::size_type{0});
}

TEST(Registry, CleanPersistentViewAfterReset) {
    entt::Registry registry;
    auto view = registry.view<int, char>(entt::persistent_t{});

    const auto entity = registry.create();
    registry.assign<int>(entity, 0);
    registry.assign<char>(entity, 'c');

    ASSERT_EQ(view.size(), entt::Registry<>::size_type{1});

    registry.reset();

    ASSERT_EQ(view.size(), entt::Registry<>::size_type{0});
}

TEST(Registry, CleanRawViewAfterReset) {
    entt::Registry registry;
    auto view = registry.view<int>(entt::raw_t{});
    registry.assign<int>(registry.create(), 0);

    ASSERT_EQ(view.size(), entt::Registry<>::size_type{1});

    registry.reset();

    ASSERT_EQ(view.size(), entt::Registry<>::size_type{0});
}

TEST(Registry, CleanTagsAfterReset) {
    entt::Registry registry;
    const auto entity = registry.create();
    registry.assign<int>(entt::tag_t{}, entity);

    ASSERT_TRUE(registry.has<int>());

    registry.reset();

    ASSERT_FALSE(registry.has<int>());
}

TEST(Registry, SortSingle) {
    entt::Registry registry;

    int val = 0;

    registry.assign<int>(registry.create(), val++);
    registry.assign<int>(registry.create(), val++);
    registry.assign<int>(registry.create(), val++);

    for(auto entity: registry.view<int>()) {
        ASSERT_EQ(registry.get<int>(entity), --val);
    }

    registry.sort<int>(std::less<int>{});

    for(auto entity: registry.view<int>()) {
        ASSERT_EQ(registry.get<int>(entity), val++);
    }
}

TEST(Registry, SortMulti) {
    entt::Registry registry;

    unsigned int uval = 0u;
    int ival = 0;

    for(auto i = 0; i < 3; ++i) {
        const auto entity = registry.create();
        registry.assign<unsigned int>(entity, uval++);
        registry.assign<int>(entity, ival++);
    }

    for(auto entity: registry.view<unsigned int>()) {
        ASSERT_EQ(registry.get<unsigned int>(entity), --uval);
    }

    for(auto entity: registry.view<int>()) {
        ASSERT_EQ(registry.get<int>(entity), --ival);
    }

    registry.sort<unsigned int>(std::less<unsigned int>{});
    registry.sort<int, unsigned int>();

    for(auto entity: registry.view<unsigned int>()) {
        ASSERT_EQ(registry.get<unsigned int>(entity), uval++);
    }

    for(auto entity: registry.view<int>()) {
        ASSERT_EQ(registry.get<int>(entity), ival++);
    }
}

TEST(Registry, ComponentsWithTypesFromStandardTemplateLibrary) {
    // see #37 - the test shouldn't crash, that's all
    entt::Registry registry;
    const auto entity = registry.create();
    registry.assign<std::unordered_set<int>>(entity).insert(42);
    registry.destroy(entity);
}

TEST(Registry, ConstructWithComponents) {
    // it should compile, that's all
    entt::Registry registry;
    const auto value = 0;
    registry.assign<int>(registry.create(), value);
}

TEST(Registry, MergeTwoRegistries) {
    using entity_type = entt::Registry<>::entity_type;

    entt::Registry src;
    entt::Registry dst;

    std::unordered_map<entity_type, entity_type> ref;

    auto merge = [&ref](const auto &view, auto &dst) {
        view.each([&](auto entity, const auto &component) {
            if(ref.find(entity) == ref.cend()) {
                const auto other = dst.create();
                dst.template assign<std::decay_t<decltype(component)>>(other, component);
                ref.emplace(entity, other);
            } else {
                using component_type = std::decay_t<decltype(component)>;
                dst.template assign<component_type>(ref[entity], component);
            }
        });
    };

    auto e0 = src.create();
    src.assign<int>(e0);
    src.assign<float>(e0);
    src.assign<double>(e0);

    auto e1 = src.create();
    src.assign<char>(e1);
    src.assign<float>(e1);
    src.assign<int>(e1);

    auto e2 = dst.create();
    dst.assign<int>(e2);
    dst.assign<char>(e2);
    dst.assign<double>(e2);

    auto e3 = dst.create();
    dst.assign<float>(e3);
    dst.assign<int>(e3);

    auto eq = [](auto begin, auto end) { ASSERT_EQ(begin, end); };
    auto ne = [](auto begin, auto end) { ASSERT_NE(begin, end); };

    eq(dst.view<int, float, double>().begin(), dst.view<int, float, double>().end());
    eq(dst.view<char, float, int>().begin(), dst.view<char, float, int>().end());

    merge(src.view<int>(), dst);
    merge(src.view<char>(), dst);
    merge(src.view<double>(), dst);
    merge(src.view<float>(), dst);

    ne(dst.view<int, float, double>().begin(), dst.view<int, float, double>().end());
    ne(dst.view<char, float, int>().begin(), dst.view<char, float, int>().end());
}

TEST(Registry, ComponentSignals) {
    entt::Registry registry;
    Listener listener;

    registry.construction<int>().connect<&Listener::incrComponent<int>>(&listener);
    registry.destruction<int>().connect<&Listener::decrComponent<int>>(&listener);

    auto e0 = registry.create();
    auto e1 = registry.create();

    registry.assign<int>(e0);
    registry.assign<int>(e1);

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, e1);

    registry.remove<int>(e0);

    ASSERT_EQ(listener.counter, 1);
    ASSERT_EQ(listener.last, e0);

    registry.destruction<int>().disconnect<&Listener::decrComponent<int>>(&listener);
    registry.remove<int>(e1);

    ASSERT_EQ(listener.counter, 1);
    ASSERT_EQ(listener.last, e0);

    registry.construction<int>().disconnect<&Listener::incrComponent<int>>(&listener);
    registry.assign<int>(e1);

    ASSERT_EQ(listener.counter, 1);
    ASSERT_EQ(listener.last, e0);

    registry.construction<int>().connect<&Listener::incrComponent<int>>(&listener);
    registry.destruction<int>().connect<&Listener::decrComponent<int>>(&listener);
    registry.assign<int>(e0);
    registry.reset<int>(e1);

    ASSERT_EQ(listener.counter, 1);
    ASSERT_EQ(listener.last, e1);

    registry.reset<int>();

    ASSERT_EQ(listener.counter, 0);
    ASSERT_EQ(listener.last, e0);

    registry.assign<int>(e0);
    registry.assign<int>(e1);
    registry.destroy(e1);

    ASSERT_EQ(listener.counter, 1);
    ASSERT_EQ(listener.last, e1);
}

TEST(Registry, TagSignals) {
    entt::Registry registry;
    Listener listener;

    registry.construction<int>(entt::tag_t{}).connect<&Listener::incrTag<int>>(&listener);
    registry.destruction<int>(entt::tag_t{}).connect<&Listener::decrTag<int>>(&listener);

    auto e0 = registry.create();
    registry.assign<int>(entt::tag_t{}, e0);

    ASSERT_EQ(listener.counter, 1);
    ASSERT_EQ(listener.last, e0);

    auto e1 = registry.create();
    registry.move<int>(e1);
    registry.remove<int>();

    ASSERT_EQ(listener.counter, 0);
    ASSERT_EQ(listener.last, e1);

    registry.construction<int>(entt::tag_t{}).disconnect<&Listener::incrTag<int>>(&listener);
    registry.destruction<int>(entt::tag_t{}).disconnect<&Listener::decrTag<int>>(&listener);
    registry.assign<int>(entt::tag_t{}, e0);
    registry.remove<int>();

    ASSERT_EQ(listener.counter, 0);
    ASSERT_EQ(listener.last, e1);

    registry.construction<int>(entt::tag_t{}).connect<&Listener::incrTag<int>>(&listener);
    registry.destruction<int>(entt::tag_t{}).connect<&Listener::decrTag<int>>(&listener);

    registry.assign<int>(entt::tag_t{}, e0);
    registry.destroy(e0);

    ASSERT_EQ(listener.counter, 0);
    ASSERT_EQ(listener.last, e0);
}

TEST(Registry, DestroyByTagAndComponents) {
    entt::Registry registry;

    const auto e0 = registry.create();
    const auto e1 = registry.create();
    const auto e2 = registry.create();
    const auto e3 = registry.create();

    registry.assign<int>(e0);
    registry.assign<char>(e0);
    registry.assign<double>(e0);

    registry.assign<int>(e1);
    registry.assign<char>(e1);

    registry.assign<int>(e2);

    registry.assign<float>(entt::tag_t{}, e3);

    ASSERT_TRUE(registry.valid(e0));
    ASSERT_TRUE(registry.valid(e1));
    ASSERT_TRUE(registry.valid(e2));
    ASSERT_TRUE(registry.valid(e3));

    registry.destroy<int, char, double>(entt::persistent_t{});

    ASSERT_FALSE(registry.valid(e0));
    ASSERT_TRUE(registry.valid(e1));
    ASSERT_TRUE(registry.valid(e2));
    ASSERT_TRUE(registry.valid(e3));

    registry.destroy<int, char>();

    ASSERT_FALSE(registry.valid(e0));
    ASSERT_FALSE(registry.valid(e1));
    ASSERT_TRUE(registry.valid(e2));
    ASSERT_TRUE(registry.valid(e3));

    registry.destroy<int>();

    ASSERT_FALSE(registry.valid(e0));
    ASSERT_FALSE(registry.valid(e1));
    ASSERT_FALSE(registry.valid(e2));
    ASSERT_TRUE(registry.valid(e3));

    registry.destroy<int>(entt::tag_t{});
    registry.destroy<char>(entt::tag_t{});
    registry.destroy<double>(entt::tag_t{});
    registry.destroy<float>(entt::tag_t{});
}
